#include <glib.h>
#include <shoveler/camera/perspective.h>
#include <shoveler/canvas.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/font.h>
#include <shoveler/font_atlas.h>
#include <shoveler/font_atlas_texture.h>
#include <shoveler/framebuffer.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image.h>
#include <shoveler/image/png.h>
#include <shoveler/input.h>
#include <shoveler/material/canvas.h>
#include <shoveler/material/text.h>
#include <shoveler/material/texture_sprite.h>
#include <shoveler/model.h>
#include <shoveler/opengl.h>
#include <shoveler/sampler.h>
#include <shoveler/scene.h>
#include <shoveler/shader_program.h>
#include <shoveler/sprite/text.h>
#include <shoveler/sprite/texture.h>
#include <shoveler/text_texture_renderer.h>
#include <shoveler/texture.h>
#include <shoveler/tileset.h>
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

static double frameTimeSinceLastUpdate = 0.0;
static unsigned int framesSinceLastUpdate = 0;
static double exponentialAverageFps = 0.0;
static GString* fpsString;
static ShovelerSprite* screenspaceTextSprite;
static ShovelerFontAtlasTexture* fontAtlasTexture;

static void shovelerSampleUpdate(ShovelerGame* game, double dt);

int main(int argc, char* argv[]) {
  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
  shovelerGlobalInit();

  if (argc != 2) {
    shovelerLogError("Usage: %s [font file]", argv[0]);
    return EXIT_FAILURE;
  }

  const char* filename = argv[1];

  ShovelerFonts* fonts = shovelerFontsCreate();
  ShovelerFont* font = shovelerFontsLoadFontFile(fonts, "test font", filename);
  if (font == NULL) {
    shovelerFontsFree(fonts);
    return EXIT_FAILURE;
  }

  ShovelerGameWindowSettings windowSettings;
  windowSettings.windowTitle = "shoveler";
  windowSettings.fullscreen = false;
  windowSettings.vsync = true;
  windowSettings.samples = 4;
  windowSettings.windowedWidth = 640;
  windowSettings.windowedHeight = 480;

  ShovelerGameCameraSettings cameraSettings;
  cameraSettings.frame.position = shovelerVector3(0, 0, 1);
  cameraSettings.frame.direction = shovelerVector3(0, 0, -1);
  cameraSettings.frame.up = shovelerVector3(0, 1, 0);
  cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
  cameraSettings.projection.aspectRatio =
      (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
  cameraSettings.projection.nearClippingPlane = 0.01;
  cameraSettings.projection.farClippingPlane = 1000;

  ShovelerGameControllerSettings controllerSettings;
  controllerSettings.frame = cameraSettings.frame;
  controllerSettings.moveFactor = 0.5f;
  controllerSettings.tiltFactor = 0.0005f;
  controllerSettings.boundingBoxSize2 = 0.2f;
  controllerSettings.boundingBoxSize3 = 0.0f;

  ShovelerGame* game = shovelerGameCreate(
      shovelerSampleUpdate, &windowSettings, &cameraSettings, &controllerSettings);
  if (game == NULL) {
    return EXIT_FAILURE;
  }

  game->controller->lockTiltX = true;
  game->controller->lockTiltY = true;

  fpsString = g_string_new("");

  ShovelerCanvas* canvas = shovelerCanvasCreate(/* numLayers */ 1);

  ShovelerFontAtlas* fontAtlas = shovelerFontAtlasCreate(font, /* fontSize */ 48, /* padding */ 1);
  fontAtlasTexture = shovelerFontAtlasTextureCreate(fontAtlas);

  ShovelerTextTextureRenderer* textTextureRenderer =
      shovelerTextTextureRendererCreate(fontAtlasTexture, game->shaderCache);
  ShovelerTexture* shovelerTextTexture =
      shovelerTextTextureRendererRender(textTextureRenderer, "shoveler", &game->renderState);
  ShovelerSampler* textureSampler =
      shovelerSamplerCreate(/* interpolate */ true, /* useMipmaps */ true, /* clamp */ true);

  ShovelerMaterial* textureSpriteMaterial = shovelerMaterialTextureSpriteCreate(
      game->shaderCache, /* screenspace */ false, SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK);
  shovelerMaterialTextureSpriteSetColor(
      textureSpriteMaterial, shovelerVector4(0.0f, 1.0f, 0.0f, 1.0f));
  ShovelerSprite* textSprite =
      shovelerSpriteTextureCreate(textureSpriteMaterial, shovelerTextTexture, textureSampler);
  shovelerSpriteUpdatePosition(textSprite, shovelerVector2(0.5f, 0.5f));
  shovelerSpriteUpdateSize(
      textSprite,
      shovelerVector2(1.0f, (float) shovelerTextTexture->height / shovelerTextTexture->width));
  shovelerCanvasAddSprite(canvas, /* layerId */ 0, textSprite);

  ShovelerMaterial* textMaterial =
      shovelerMaterialTextCreate(game->shaderCache, /* screenspace */ true);
  screenspaceTextSprite = shovelerSpriteTextCreate(
      textMaterial,
      fontAtlasTexture,
      /* fontSize */ 48.0f,
      /* color */ shovelerVector4(0.0f, 1.0f, 0.0f, 0.5f));
  shovelerCanvasAddSprite(game->screenspaceCanvas, /* layerId */ 0, screenspaceTextSprite);

  shovelerImagePngWriteFile(fontAtlas->image, "atlas.png");

  ShovelerMaterial* canvasMaterial =
      shovelerMaterialCanvasCreate(game->shaderCache, /* screenspace */ false);
  shovelerMaterialCanvasSetActive(canvasMaterial, canvas);
  shovelerMaterialCanvasSetActiveRegion(
      canvasMaterial, shovelerVector2(0.5f, 0.5f), shovelerVector2(1.0f, 1.0f));
  ShovelerDrawable* quad = shovelerDrawableQuadCreate();
  ShovelerModel* canvasModel = shovelerModelCreate(quad, canvasMaterial);
  canvasModel->scale = shovelerVector3(0.5, 0.5, 1.0);
  canvasModel->emitter = true;
  shovelerModelUpdateTransformation(canvasModel);
  shovelerSceneAddModel(game->scene, canvasModel);

  shovelerOpenGLCheckSuccess();

  while (shovelerGameIsRunning(game)) {
    shovelerGameRenderFrame(game);
  }
  shovelerLogInfo("Exiting main loop, goodbye.");

  g_string_free(fpsString, true);
  shovelerDrawableFree(quad);
  shovelerMaterialFree(canvasMaterial);
  shovelerMaterialFree(textMaterial);
  shovelerMaterialFree(textureSpriteMaterial);
  shovelerCanvasFree(canvas);
  shovelerSpriteFree(screenspaceTextSprite);
  shovelerSpriteFree(textSprite);
  shovelerSamplerFree(textureSampler);
  shovelerTextureFree(shovelerTextTexture);
  shovelerTextTextureRendererFree(textTextureRenderer);
  shovelerFontAtlasTextureFree(fontAtlasTexture);
  shovelerFontAtlasFree(fontAtlas);
  shovelerFontsFree(fonts);
  shovelerGameFree(game);
  shovelerGlobalUninit();
  shovelerLogTerminate();

  return EXIT_SUCCESS;
}

static void shovelerSampleUpdate(ShovelerGame* game, double dt) {
  shovelerCameraUpdateView(game->camera);

  frameTimeSinceLastUpdate += dt;
  framesSinceLastUpdate++;

  if (frameTimeSinceLastUpdate > 0.1) {
    double fps = framesSinceLastUpdate / frameTimeSinceLastUpdate;
    exponentialAverageFps = 0.5 * fps + 0.5 * exponentialAverageFps;
    g_string_set_size(fpsString, 0);
    g_string_append_printf(fpsString, "FPS: %.1f", exponentialAverageFps);
    shovelerSpriteTextSetContent(screenspaceTextSprite, fpsString->str, /* copyContent */ false);
    screenspaceTextSprite->position =
        shovelerVector2(10.0f, game->framebuffer->height - 48.0f - 10.0f);

    for (const char* c = fpsString->str; *c != '\0'; c++) {
      unsigned char character = *((unsigned char*) c);
      shovelerFontAtlasGetGlyph(fontAtlasTexture->fontAtlas, character);
    }
    shovelerFontAtlasTextureUpdate(fontAtlasTexture);

    frameTimeSinceLastUpdate = 0.0;
    framesSinceLastUpdate = 0;
  }
}
