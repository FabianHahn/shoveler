#include <glib.h>
#include <shoveler/camera/perspective.h>
#include <shoveler/client_system.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/file.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image/png.h>
#include <shoveler/material/texture.h>
#include <shoveler/opengl.h>
#include <shoveler/projection.h>
#include <shoveler/resources.h>
#include <shoveler/scene.h>
#include <shoveler/schema/base.h>
#include <shoveler/schema/opengl.h>
#include <shoveler/types.h>
#include <shoveler/world.h>
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memcpy

static void updateGame(ShovelerGame* game, double dt);
static void updateAuthoritativeComponent(
    ShovelerClientSystem* clientSystem,
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData);

static ShovelerWorld* world = NULL;

int main(int argc, char* argv[]) {
  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
  shovelerGlobalInit();

  if (argc != 2) {
    shovelerLogError("Usage: %s [font file]", argv[0]);
    return EXIT_FAILURE;
  }

  const char* filename = argv[1];

  unsigned char* contents;
  size_t contentsSize;
  if (!shovelerFileRead(filename, &contents, &contentsSize)) {
    shovelerLogError("Failed to read font file.");
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
  cameraSettings.frame.position = shovelerVector3(0, 0, -5);
  cameraSettings.frame.direction = shovelerVector3(0, 0, 1);
  cameraSettings.frame.up = shovelerVector3(0, 1, 0);
  cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
  cameraSettings.projection.aspectRatio =
      (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
  cameraSettings.projection.nearClippingPlane = 0.01;
  cameraSettings.projection.farClippingPlane = 1000;

  ShovelerGameControllerSettings controllerSettings;
  controllerSettings.frame = cameraSettings.frame;
  controllerSettings.moveFactor = 2.0f;
  controllerSettings.tiltFactor = 0.0005f;
  controllerSettings.boundingBoxSize2 = 0.0f;
  controllerSettings.boundingBoxSize3 = 0.0f;

  ShovelerGame* game =
      shovelerGameCreate(updateGame, &windowSettings, &cameraSettings, &controllerSettings);
  if (game == NULL) {
    return EXIT_FAILURE;
  }

  ShovelerClientSystem* clientSystem = shovelerClientSystemCreate(
      game,
      updateAuthoritativeComponent,
      /* updateAuthoritativeComponentUserData */ NULL);
  world = clientSystem->world;

  game->controller->lockTiltX = true;
  game->controller->lockTiltY = true;

  ShovelerWorldEntity* fontEntity = shovelerWorldAddEntity(world, 1);
  ShovelerComponent* resourceComponent =
      shovelerWorldEntityAddComponent(fontEntity, shovelerComponentTypeIdResource);
  shovelerComponentUpdateCanonicalFieldBytes(
      resourceComponent, SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER, contents, contentsSize);
  shovelerComponentActivate(resourceComponent);
  ShovelerComponent* fontComponent =
      shovelerWorldEntityAddComponent(fontEntity, shovelerComponentTypeIdFont);
  shovelerComponentUpdateCanonicalFieldString(
      fontComponent, SHOVELER_COMPONENT_FONT_FIELD_ID_NAME, "OpenSans");
  shovelerComponentUpdateCanonicalFieldEntityId(
      fontComponent, SHOVELER_COMPONENT_FONT_FIELD_ID_RESOURCE, 1);
  shovelerComponentActivate(fontComponent);
  ShovelerComponent* fontAtlasComponent =
      shovelerWorldEntityAddComponent(fontEntity, shovelerComponentTypeIdFontAtlas);
  shovelerComponentUpdateCanonicalFieldEntityId(
      fontAtlasComponent, SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT, 1);
  shovelerComponentUpdateCanonicalFieldInt(
      fontAtlasComponent, SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT_SIZE, 48);
  shovelerComponentUpdateCanonicalFieldInt(
      fontAtlasComponent, SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_PADDING, 1);
  shovelerComponentActivate(fontAtlasComponent);
  ShovelerComponent* fontAtlasTextureComponent =
      shovelerWorldEntityAddComponent(fontEntity, shovelerComponentTypeIdFontAtlasTexture);
  shovelerComponentUpdateCanonicalFieldEntityId(
      fontAtlasTextureComponent, SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_FIELD_ID_FONT_ATLAS, 1);
  shovelerComponentActivate(fontAtlasTextureComponent);
  ShovelerComponent* textTextureRendererComponent =
      shovelerWorldEntityAddComponent(fontEntity, shovelerComponentTypeIdTextTextureRenderer);
  shovelerComponentUpdateCanonicalFieldEntityId(
      textTextureRendererComponent,
      SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_FIELD_ID_FONT_ATLAS_TEXTURE,
      1);
  shovelerComponentActivate(textTextureRendererComponent);

  ShovelerWorldEntity* textEntity = shovelerWorldAddEntity(world, 2);
  ShovelerComponent* textureComponent =
      shovelerWorldEntityAddComponent(textEntity, shovelerComponentTypeIdTexture);
  shovelerComponentUpdateCanonicalFieldInt(
      textureComponent,
      SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_TEXTURE_TYPE_TEXT);
  shovelerComponentUpdateCanonicalFieldEntityId(
      textureComponent, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT_TEXTURE_RENDERER, 1);
  shovelerComponentUpdateCanonicalFieldString(
      textureComponent, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT, "shoveler");
  shovelerComponentActivate(textureComponent);
  ShovelerComponent* samplerComponent =
      shovelerWorldEntityAddComponent(textEntity, shovelerComponentTypeIdSampler);
  shovelerComponentUpdateCanonicalFieldBool(
      samplerComponent, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_INTERPOLATE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      samplerComponent, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_USE_MIPMAPS, true);
  shovelerComponentUpdateCanonicalFieldBool(
      samplerComponent, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_CLAMP, true);
  shovelerComponentActivate(samplerComponent);
  ShovelerComponent* drawableComponent =
      shovelerWorldEntityAddComponent(textEntity, shovelerComponentTypeIdDrawable);
  shovelerComponentUpdateCanonicalFieldInt(
      drawableComponent,
      SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD);
  shovelerComponentActivate(drawableComponent);
  ShovelerComponent* materialComponent =
      shovelerWorldEntityAddComponent(textEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      materialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE);
  shovelerComponentUpdateCanonicalFieldInt(
      materialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_TYPE,
      SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK);
  shovelerComponentUpdateCanonicalFieldVector4(
      materialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR,
      shovelerVector4(0.2f, 0.8f, 0.2f, 1.0f));
  shovelerComponentUpdateCanonicalFieldEntityId(
      materialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE, 2);
  shovelerComponentUpdateCanonicalFieldEntityId(
      materialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SAMPLER, 2);
  shovelerComponentActivate(materialComponent);
  ShovelerComponent* modelComponent =
      shovelerWorldEntityAddComponent(textEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 2);
  shovelerComponentUpdateCanonicalFieldEntityId(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 2);
  shovelerComponentUpdateCanonicalFieldEntityId(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 2);
  shovelerComponentUpdateCanonicalFieldVector3(
      modelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION,
      shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
  shovelerComponentUpdateCanonicalFieldVector3(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE, shovelerVector3(8.0f, 1.5f, 1.0f));
  shovelerComponentUpdateCanonicalFieldBool(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, true);
  shovelerComponentUpdateCanonicalFieldBool(
      modelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, false);
  shovelerComponentUpdateCanonicalFieldInt(
      modelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(modelComponent);
  ShovelerComponent* positionComponent =
      shovelerWorldEntityAddComponent(textEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      positionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      positionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(0.0f, 0.0f, 10.0f));
  shovelerComponentActivate(positionComponent);

  shovelerOpenGLCheckSuccess();

  while (shovelerGameIsRunning(game)) {
    shovelerGameRenderFrame(game);
  }
  shovelerLogInfo("Exiting main loop, goodbye.");

  free(contents);
  shovelerClientSystemFree(clientSystem);
  shovelerGameFree(game);
  shovelerGlobalUninit();
  shovelerLogTerminate();

  return EXIT_SUCCESS;
}

static void updateGame(ShovelerGame* game, double dt) { shovelerCameraUpdateView(game->camera); }

static void updateAuthoritativeComponent(
    ShovelerClientSystem* clientSystem,
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData) {
  GString* printedValue = shovelerComponentFieldPrintValue(value);

  shovelerLogInfo(
      "Updating field '%s' of component '%s' on entity %lld: %s",
      field->name,
      component->type->id,
      component->entityId,
      printedValue->str);

  g_string_free(printedValue, true);
}
