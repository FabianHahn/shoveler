#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/material/canvas.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/framebuffer.h>
#include <shoveler/global.h>
#include <shoveler/image.h>
#include <shoveler/input.h>
#include <shoveler/model.h>
#include <shoveler/opengl.h>
#include <shoveler/scene.h>
#include <shoveler/shader_program.h>
#include <shoveler/texture.h>
#include <shoveler/tileset.h>

static void shovelerSampleInit(ShovelerGame *sampleGame, int width, int height, int samples);
static void shovelerSampleTerminate();
static void shovelerSampleUpdate(ShovelerGame *game, double dt);

int main(int argc, char *argv[])
{
	const char *windowTitle = "shoveler";
	bool fullscreen = false;
	bool vsync = true;
	int samples = 4;
	int width = 640;
	int height = 480;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerGame *game = shovelerGameCreate(windowTitle, width, height, samples, fullscreen, vsync);
	if(game == NULL) {
		return EXIT_FAILURE;
	}
	game->scene = shovelerSceneCreate();

	ShovelerController *controller = shovelerControllerCreate(game, shovelerVector3(0, 0, 1), shovelerVector3(0, 0, -1), shovelerVector3(0, 1, 0), 2.0f, 0.0005f);
	game->camera = shovelerCameraPerspectiveCreate(shovelerVector3(0, 0, 1), shovelerVector3(0, 0, -1), shovelerVector3(0, 1, 0), 2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);
	shovelerCameraPerspectiveAttachController(game->camera, controller);

	ShovelerCanvas *canvas = shovelerCanvasCreate();

	ShovelerImage *tilesetImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tilesetImage);
	shovelerImageGet(tilesetImage, 0, 0, 0) = 255; // red
	shovelerImageGet(tilesetImage, 0, 1, 1) = 255; // green
	shovelerImageGet(tilesetImage, 1, 0, 2) = 255; // blue
	shovelerImageGet(tilesetImage, 1, 1, 0) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 1) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 2) = 255; // white
	ShovelerTileset *tileset = shovelerTilesetCreate(tilesetImage, 2, 2, 1);
	ShovelerTileset *tileset2 = shovelerTilesetCreate(tilesetImage, 1, 1, 1);

	ShovelerCanvasTileSprite tileSprite;
	tileSprite.tileset = tileset;
	tileSprite.tilesetColumn = 1;
	tileSprite.tilesetRow = 1;
	tileSprite.position = shovelerVector2(2.0, 3.0);
	tileSprite.size = shovelerVector2(2.5, 4.0);
	shovelerCanvasAddTileSprite(canvas, tileSprite);

	ShovelerCanvasTileSprite tileSprite2;
	tileSprite2.tileset = tileset2;
	tileSprite2.tilesetColumn = 0;
	tileSprite2.tilesetRow = 0;
	tileSprite2.position = shovelerVector2(5.0, 5.0);
	tileSprite2.size = shovelerVector2(4.5, 4.5);
	shovelerCanvasAddTileSprite(canvas, tileSprite2);

	ShovelerMaterial *canvasMaterial = shovelerMaterialCanvasCreate(canvas, true);
	ShovelerDrawable *quad = shovelerDrawableQuadCreate();
	ShovelerModel *canvasModel = shovelerModelCreate(quad, canvasMaterial);
	canvasModel->size2d = shovelerVector2(10.0f, 10.0f);
	canvasModel->scale = shovelerVector3(0.5, 0.5, 1.0);
	canvasModel->screenspace = true;
	shovelerModelUpdateTransformation(canvasModel);
	shovelerSceneAddModel(game->scene, canvasModel);

	shovelerOpenGLCheckSuccess();

	game->update = shovelerSampleUpdate;

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerDrawableFree(quad);
	shovelerMaterialFree(canvasMaterial);
	shovelerControllerFree(controller);

	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void shovelerSampleUpdate(ShovelerGame *game, double dt)
{
	shovelerControllerUpdate(shovelerCameraPerspectiveGetController(game->camera), dt);
	shovelerCameraUpdateView(game->camera);
}
