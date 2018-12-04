#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/material/chunk.h>
#include <shoveler/canvas.h>
#include <shoveler/chunk.h>
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
#include <shoveler/tilemap.h>
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

	ShovelerChunk *chunk = shovelerChunkCreate();

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
	ShovelerTexture *tilesTexture = shovelerTextureCreate2d(tilesImage, true);
	shovelerTextureUpdate(tilesTexture);
	ShovelerTilemap *tilemap = shovelerTilemapCreate(tilesTexture);
	shovelerChunkAddTilemapLayer(chunk, tilemap);

	ShovelerCanvas *canvas = shovelerCanvasCreate();
	shovelerChunkAddCanvasLayer(chunk, canvas);

	ShovelerImage *borderTilesImage = shovelerImageCreate(1, 1, 3);
	shovelerImageClear(borderTilesImage);
	shovelerImageGet(borderTilesImage, 0, 0, 0) = 0;
	shovelerImageGet(borderTilesImage, 0, 0, 1) = 0;
	shovelerImageGet(borderTilesImage, 0, 0, 2) = 1; // full tileset
	ShovelerTexture *borderTilesTexture = shovelerTextureCreate2d(borderTilesImage, true);
	shovelerTextureUpdate(borderTilesTexture);
	ShovelerTilemap *borderTilemap = shovelerTilemapCreate(borderTilesTexture);
	shovelerChunkAddTilemapLayer(chunk, borderTilemap);

	ShovelerImage *tilesetImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(tilesetImage);
	shovelerImageGet(tilesetImage, 0, 0, 0) = 255; // red
	shovelerImageGet(tilesetImage, 0, 1, 1) = 255; // green
	shovelerImageGet(tilesetImage, 1, 0, 2) = 255; // blue
	shovelerImageGet(tilesetImage, 1, 1, 0) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 1) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 2) = 255; // white
	ShovelerTileset *tileset = shovelerTilesetCreate(tilesetImage, 2, 2, 1);
	shovelerTilemapAddTileset(tilemap, tileset);
	ShovelerTileset *tileset2 = shovelerTilesetCreate(tilesetImage, 1, 1, 1);
	shovelerTilemapAddTileset(tilemap, tileset2);
	shovelerImageFree(tilesetImage);

	ShovelerImage *borderTilesetImage = shovelerImageCreate(20, 20, 4);
	shovelerImageClear(borderTilesetImage);
	shovelerImageSet(borderTilesetImage, shovelerColor(255, 200, 255), 0);
	shovelerImageAddFrame(borderTilesetImage, 5, shovelerColor(255, 200, 255));
	ShovelerTileset *borderTileset = shovelerTilesetCreate(borderTilesetImage, 1, 1, 1);
	shovelerImageFree(borderTilesetImage);
	shovelerTilemapAddTileset(borderTilemap, borderTileset);

	ShovelerCanvasTileSprite tileSprite;
	tileSprite.tileset = tileset;
	tileSprite.tilesetColumn = 1;
	tileSprite.tilesetRow = 0;
	tileSprite.position = shovelerVector2(3.5, 3.5);
	tileSprite.size = shovelerVector2(5.0, 5.0);
	shovelerCanvasAddTileSprite(canvas, tileSprite);

	ShovelerMaterial *chunkMaterial = shovelerMaterialChunkCreate(chunk, true);
	ShovelerDrawable *quad = shovelerDrawableQuadCreate();
	ShovelerModel *chunkModel = shovelerModelCreate(quad, chunkMaterial);
	chunkModel->size2d = shovelerVector2(10.0f, 10.0f);
	chunkModel->scale = shovelerVector3(0.5, 0.5, 1.0);
	chunkModel->screenspace = true;
	shovelerModelUpdateTransformation(chunkModel);
	shovelerSceneAddModel(game->scene, chunkModel);

	shovelerOpenGLCheckSuccess();

	game->update = shovelerSampleUpdate;

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerDrawableFree(quad);
	shovelerMaterialFree(chunkMaterial);
	shovelerCanvasFree(canvas);
	shovelerTilemapFree(tilemap);
	shovelerTilesetFree(tileset);
	shovelerTilesetFree(tileset2);
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