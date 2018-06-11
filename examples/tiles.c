#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/tiles.h>
#include <shoveler/material/tilemap.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/framebuffer.h>
#include <shoveler/global.h>
#include <shoveler/image.h>
#include <shoveler/input.h>
#include <shoveler/model.h>
#include <shoveler/opengl.h>
#include <shoveler/sampler.h>
#include <shoveler/scene.h>
#include <shoveler/shader_program.h>
#include <shoveler/texture.h>

static float previousCursorX;
static float previousCursorY;

static float eps = 1e-9;

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

	ShovelerController *controller = shovelerControllerCreate(game, (ShovelerVector3){0.5, 0.5, 1}, (ShovelerVector3){0, 0, -1}, (ShovelerVector3){0, 1, 0}, 2.0f, 0.0005f);
	game->camera = shovelerCameraPerspectiveCreate((ShovelerVector3){0.5, 0.5, 1}, (ShovelerVector3){0, 0, -1}, (ShovelerVector3){0, 1, 0}, 2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);
	shovelerCameraPerspectiveAttachController(game->camera, controller);

	ShovelerSampler *nearestNeighborSampler = shovelerSamplerCreate(false, true);
	ShovelerSampler *interpolatingSampler = shovelerSamplerCreate(true, true);

	ShovelerImage *tileset = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tileset);
	shovelerImageGet(tileset, 0, 0, 0) = 255; // red
	shovelerImageGet(tileset, 0, 1, 1) = 255; // green
	shovelerImageGet(tileset, 1, 0, 2) = 255; // blue
	shovelerImageGet(tileset, 1, 1, 0) = 255;
	shovelerImageGet(tileset, 1, 1, 1) = 255;
	shovelerImageGet(tileset, 1, 1, 2) = 255; // white
	ShovelerTexture *tilesetTexture = shovelerTextureCreate2d(tileset);
	shovelerTextureUpdate(tilesetTexture);

	ShovelerImage *tilemap = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tilemap);
	shovelerImageGet(tilemap, 0, 0, 0) = 0;
	shovelerImageGet(tilemap, 0, 0, 1) = 0;
	shovelerImageGet(tilemap, 0, 0, 2) = 0; // red
	shovelerImageGet(tilemap, 0, 1, 0) = 0;
	shovelerImageGet(tilemap, 0, 1, 1) = 0;
	shovelerImageGet(tilemap, 0, 1, 2) = 0; // red
	shovelerImageGet(tilemap, 1, 0, 0) = 0;
	shovelerImageGet(tilemap, 1, 0, 1) = 1;
	shovelerImageGet(tilemap, 1, 0, 2) = 0; // green
	shovelerImageGet(tilemap, 1, 1, 0) = 0;
	shovelerImageGet(tilemap, 1, 1, 1) = 0;
	shovelerImageGet(tilemap, 1, 1, 2) = 1; // n/a
	ShovelerTexture *tilemapTexture = shovelerTextureCreate2d(tilemap);
	shovelerTextureUpdate(tilemapTexture);

	ShovelerDrawable *tiles = shovelerDrawableTilesCreate(2, 2);
	ShovelerMaterial *tilemapMaterial = shovelerMaterialTilemapCreate();
	shovelerMaterialTilemapSetSource(tilemapMaterial, tilemapTexture);
	shovelerMaterialTilemapSetTileset(tilemapMaterial, 0, tilesetTexture, nearestNeighborSampler, 2, 2);

	ShovelerModel *tilesModel = shovelerModelCreate(tiles, tilemapMaterial);
	tilesModel->screenspace = true;
	shovelerSceneAddModel(game->scene, tilesModel);

	shovelerOpenGLCheckSuccess();

	double newCursorX;
	double newCursorY;
	glfwGetCursorPos(game->window, &newCursorX, &newCursorY);
	previousCursorX = newCursorX;
	previousCursorY = newCursorY;

	game->update = shovelerSampleUpdate;

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerDrawableFree(tiles);
	shovelerMaterialFree(tilemapMaterial);
	shovelerSamplerFree(interpolatingSampler);
	shovelerSamplerFree(nearestNeighborSampler);
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
