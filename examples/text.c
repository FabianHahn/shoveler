#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/image/png.h>
#include <shoveler/material/canvas.h>
#include <shoveler/canvas.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/font_atlas.h>
#include <shoveler/font_atlas_texture.h>
#include <shoveler/font.h>
#include <shoveler/framebuffer.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image.h>
#include <shoveler/input.h>
#include <shoveler/model.h>
#include <shoveler/opengl.h>
#include <shoveler/scene.h>
#include <shoveler/shader_program.h>
#include <shoveler/texture.h>

static void shovelerSampleUpdate(ShovelerGame *game, double dt);

int main(int argc, char *argv[])
{
	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	if(argc != 2) {
		shovelerLogError("Usage: %s [font file]", argv[0]);
		return EXIT_FAILURE;
	}

	const char *filename = argv[1];

	ShovelerFonts *fonts = shovelerFontsCreate();
	ShovelerFont *font = shovelerFontsLoadFont(fonts, "test font", filename);
	if(font == NULL) {
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
	cameraSettings.projection.aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
	cameraSettings.projection.nearClippingPlane = 0.01;
	cameraSettings.projection.farClippingPlane = 1000;

	ShovelerGameControllerSettings controllerSettings;
	controllerSettings.frame = cameraSettings.frame;
	controllerSettings.moveFactor = 0.5f;
	controllerSettings.tiltFactor = 0.0005f;
	controllerSettings.boundingBoxSize2 = 0.2f;
	controllerSettings.boundingBoxSize3 = 0.0f;

	ShovelerGame *game = shovelerGameCreate(shovelerSampleUpdate, &windowSettings, &cameraSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	game->controller->lockTiltX = true;
	game->controller->lockTiltY = true;

	ShovelerCanvas *canvas = shovelerCanvasCreate();

	ShovelerFontAtlas *fontAtlas = shovelerFontAtlasCreate(font, 48, 1);
	ShovelerFontAtlasTexture *fontAtlasTexture = shovelerFontAtlasTextureCreate(fontAtlas);

	ShovelerCanvasTextSprite textSprite;
	textSprite.fontAtlasTexture = fontAtlasTexture;
	textSprite.text = "shoveler";
	textSprite.position = shovelerVector2(0.0f, 0.25f);
	textSprite.size = 0.25f;
	shovelerCanvasAddTextSprite(canvas, &textSprite);

	for(const char *c = textSprite.text; *c != '\0'; c++) {
		unsigned char character = *((unsigned char *) c);
		shovelerFontAtlasGetGlyph(fontAtlas, character);
	}
	shovelerFontAtlasTextureUpdate(fontAtlasTexture);
	shovelerImagePngWriteFile(fontAtlas->image, "atlas.png");

	ShovelerMaterial *canvasMaterial = shovelerMaterialCanvasCreate(game->shaderCache, false);
	shovelerMaterialCanvasSetActive(canvasMaterial, canvas);
	shovelerMaterialCanvasSetActiveRegion(canvasMaterial, shovelerVector2(0.5f, 0.5f), shovelerVector2(1.0f, 1.0f));
	ShovelerDrawable *quad = shovelerDrawableQuadCreate();
	ShovelerModel *canvasModel = shovelerModelCreate(quad, canvasMaterial);
	canvasModel->scale = shovelerVector3(0.5, 0.5, 1.0);
	canvasModel->screenspace = true;
	shovelerModelUpdateTransformation(canvasModel);
	shovelerSceneAddModel(game->scene, canvasModel);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerDrawableFree(quad);
	shovelerMaterialFree(canvasMaterial);
	shovelerCanvasFree(canvas);
	shovelerFontAtlasTextureFree(fontAtlasTexture);
	shovelerFontAtlasFree(fontAtlas);
	shovelerFontsFree(fonts);
	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void shovelerSampleUpdate(ShovelerGame *game, double dt)
{
	ShovelerController *controller = shovelerCameraPerspectiveGetController(game->camera);

	shovelerCameraUpdateView(game->camera);
}
