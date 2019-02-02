#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/material/tilemap.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
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

static void shovelerSampleTerminate();
static void shovelerSampleUpdate(ShovelerGame *game, double dt);

int main(int argc, char *argv[])
{
	ShovelerGameWindowSettings windowSettings;
	windowSettings.windowTitle = "shoveler";
	windowSettings.fullscreen = false;
	windowSettings.vsync = true;
	windowSettings.samples = 4;
	windowSettings.windowedWidth = 640;
	windowSettings.windowedHeight = 480;

	ShovelerGameControllerSettings controllerSettings;
	controllerSettings.frame.position = shovelerVector3(0, 0, 1);
	controllerSettings.frame.direction = shovelerVector3(0, 0, -1);
	controllerSettings.frame.up = shovelerVector3(0, 1, 0);
	controllerSettings.moveFactor = 0.5f;
	controllerSettings.tiltFactor = 0.0005f;

	ShovelerProjectionPerspective projection;
	projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
	projection.aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
	projection.nearClippingPlane = 0.01;
	projection.farClippingPlane = 1000;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerCamera *camera = shovelerCameraPerspectiveCreate(&controllerSettings.frame, &projection);

	ShovelerGame *game = shovelerGameCreate(camera, shovelerSampleUpdate, &windowSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	game->controller->lockTiltX = true;
	game->controller->lockTiltY = true;
	shovelerCameraPerspectiveAttachController(camera, game->controller);

	ShovelerImage *tilesetImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tilesetImage);
	shovelerImageGet(tilesetImage, 0, 0, 0) = 255; // red
	shovelerImageGet(tilesetImage, 0, 1, 1) = 255; // green
	shovelerImageGet(tilesetImage, 1, 0, 2) = 255; // blue
	shovelerImageGet(tilesetImage, 1, 1, 0) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 1) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 2) = 255; // white

	ShovelerImage *borderTilesetImage = shovelerImageCreate(20, 20, 4);
	shovelerImageClear(borderTilesetImage);
	shovelerImageSet(borderTilesetImage, shovelerColor(255, 200, 255), 0);
	shovelerImageAddFrame(borderTilesetImage, 3, shovelerColor(255, 200, 255));

	ShovelerImage *tilesImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tilesImage);
	shovelerImageGet(tilesImage, 0, 0, 0) = 0;
	shovelerImageGet(tilesImage, 0, 0, 1) = 0;
	shovelerImageGet(tilesImage, 0, 0, 2) = 1; // red
	shovelerImageGet(tilesImage, 0, 1, 0) = 0;
	shovelerImageGet(tilesImage, 0, 1, 1) = 0;
	shovelerImageGet(tilesImage, 0, 1, 2) = 1; // red
	shovelerImageGet(tilesImage, 1, 0, 0) = 0;
	shovelerImageGet(tilesImage, 1, 0, 1) = 1;
	shovelerImageGet(tilesImage, 1, 0, 2) = 1; // green
	shovelerImageGet(tilesImage, 1, 1, 0) = 0;
	shovelerImageGet(tilesImage, 1, 1, 1) = 0;
	shovelerImageGet(tilesImage, 1, 1, 2) = 2; // full tileset
	ShovelerTexture *tiles = shovelerTextureCreate2d(tilesImage, true);
	shovelerTextureUpdate(tiles);

	ShovelerTilemap *tilemap = shovelerTilemapCreate(tiles);
	ShovelerTileset *tileset = shovelerTilesetCreate(tilesetImage, 2, 2, 1);
	shovelerTilemapAddTileset(tilemap, tileset);
	ShovelerTileset *tileset2 = shovelerTilesetCreate(tilesetImage, 1, 1, 1);
	shovelerTilemapAddTileset(tilemap, tileset2);

	ShovelerMaterial *tilemapMaterial = shovelerMaterialTilemapCreate();
	shovelerMaterialTilemapSetActive(tilemapMaterial, tilemap);

	ShovelerDrawable *quad = shovelerDrawableQuadCreate();
	ShovelerModel *tilesModel = shovelerModelCreate(quad, tilemapMaterial);
	tilesModel->scale = shovelerVector3(0.5, 0.5, 1.0);
	tilesModel->screenspace = true;
	shovelerModelUpdateTransformation(tilesModel);
	shovelerSceneAddModel(game->scene, tilesModel);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerDrawableFree(quad);
	shovelerMaterialFree(tilemapMaterial);
	shovelerCameraPerspectiveDetachController(camera);
	shovelerGameFree(game);
	shovelerCameraFree(camera);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void shovelerSampleUpdate(ShovelerGame *game, double dt)
{
	shovelerCameraUpdateView(game->camera);
}
