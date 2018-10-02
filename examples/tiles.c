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

	ShovelerMaterial *tilemapMaterial = shovelerMaterialTilemapCreate(2, 2);

	ShovelerImage *tilesetImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tilesetImage);
	shovelerImageGet(tilesetImage, 0, 0, 0) = 255; // red
	shovelerImageGet(tilesetImage, 0, 1, 1) = 255; // green
	shovelerImageGet(tilesetImage, 1, 0, 2) = 255; // blue
	shovelerImageGet(tilesetImage, 1, 1, 0) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 1) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 2) = 255; // white
	ShovelerTexture *tilesetTexture = shovelerTextureCreate2d(tilesetImage, true);
	shovelerTextureUpdate(tilesetTexture);

	ShovelerMaterialTilemapTileset tileset = {2, 2, tilesetTexture, nearestNeighborSampler};
	shovelerMaterialTilemapAddTileset(tilemapMaterial, tileset);
	ShovelerMaterialTilemapTileset tileset2 = {1, 1, tilesetTexture, interpolatingSampler};
	shovelerMaterialTilemapAddTileset(tilemapMaterial, tileset2);

	ShovelerImage *layerImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(layerImage);
	shovelerImageGet(layerImage, 0, 0, 0) = 0;
	shovelerImageGet(layerImage, 0, 0, 1) = 0;
	shovelerImageGet(layerImage, 0, 0, 2) = 1; // red
	shovelerImageGet(layerImage, 0, 1, 0) = 0;
	shovelerImageGet(layerImage, 0, 1, 1) = 0;
	shovelerImageGet(layerImage, 0, 1, 2) = 1; // red
	shovelerImageGet(layerImage, 1, 0, 0) = 0;
	shovelerImageGet(layerImage, 1, 0, 1) = 1;
	shovelerImageGet(layerImage, 1, 0, 2) = 1; // green
	shovelerImageGet(layerImage, 1, 1, 0) = 0;
	shovelerImageGet(layerImage, 1, 1, 1) = 0;
	shovelerImageGet(layerImage, 1, 1, 2) = 2; // full tileset
	ShovelerTexture *layerTexture = shovelerTextureCreate2d(layerImage, true);
	shovelerTextureUpdate(layerTexture);
	shovelerMaterialTilemapAddLayer(tilemapMaterial, layerTexture);

	ShovelerImage *layer2Image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(layer2Image);
	shovelerImageGet(layer2Image, 0, 0, 0) = 0;
	shovelerImageGet(layer2Image, 0, 0, 1) = 0;
	shovelerImageGet(layer2Image, 0, 0, 2) = 2; // full tileset
	ShovelerTexture *layer2Texture = shovelerTextureCreate2d(layer2Image, true);
	shovelerTextureUpdate(layer2Texture);
	shovelerMaterialTilemapAddLayer(tilemapMaterial, layer2Texture);

	ShovelerDrawable *tiles = shovelerDrawableTilesCreate(2, 2);
	ShovelerModel *tilesModel = shovelerModelCreate(tiles, tilemapMaterial);
	tilesModel->screenspace = true;
	shovelerSceneAddModel(game->scene, tilesModel);

	shovelerOpenGLCheckSuccess();

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
