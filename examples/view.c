#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/image/png.h>
#include <shoveler/resources/image_png.h>
#include <shoveler/view/canvas.h>
#include <shoveler/view/chunk.h>
#include <shoveler/view/drawable.h>
#include <shoveler/view/light.h>
#include <shoveler/view/material.h>
#include <shoveler/view/model.h>
#include <shoveler/view/position.h>
#include <shoveler/view/resources.h>
#include <shoveler/view/texture.h>
#include <shoveler/view/tile_sprite.h>
#include <shoveler/view/tilemap.h>
#include <shoveler/view/tilemap_tiles.h>
#include <shoveler/view/tileset.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/file.h>
#include <shoveler/global.h>
#include <shoveler/opengl.h>
#include <shoveler/resources.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

static double time = 0.0;
static ShovelerView *view = NULL;

static GString *getImageData(ShovelerImage *image);
static void updateGame(ShovelerGame *game, double dt);

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

	game->camera = shovelerCameraPerspectiveCreate(shovelerVector3(0.0, 0.0, -5.0), shovelerVector3(0.0, 0.0, 1.0), shovelerVector3(0.0, 1.0, 0.0), 2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);
	game->scene = shovelerSceneCreate();
	game->update = updateGame;

	ShovelerController *controller = shovelerControllerCreate(game, shovelerVector3(0.0, 0.0, -5.0), shovelerVector3(0.0, 0.0, 1.0), shovelerVector3(0.0, 1.0, 0.0), 2.0f, 0.0005f);
	shovelerCameraPerspectiveAttachController(game->camera, controller);

	ShovelerResources *resources = shovelerResourcesCreate(NULL, NULL);
	shovelerResourcesImagePngRegister(resources);

	view = shovelerViewCreate();
	shovelerViewSetTarget(view, "controller", controller);
	shovelerViewSetResources(view, resources);
	shovelerViewSetTarget(view, "scene", game->scene);

	ShovelerViewDrawableConfiguration cubeDrawableConfiguration;
	cubeDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_CUBE;
	ShovelerViewEntity *cubeDrawableEntity = shovelerViewAddEntity(view, 1);
	shovelerViewEntityAddDrawable(cubeDrawableEntity, cubeDrawableConfiguration);

	ShovelerViewMaterialConfiguration grayColorMaterialConfiguration;
	grayColorMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_COLOR;
	grayColorMaterialConfiguration.color = shovelerVector3(0.7, 0.7, 0.7);
	ShovelerViewEntity *grayColorMaterialEntity = shovelerViewAddEntity(view, 2);
	shovelerViewEntityAddMaterial(grayColorMaterialEntity, grayColorMaterialConfiguration);

	ShovelerViewModelConfiguration cubeModelConfiguration;
	cubeModelConfiguration.drawableEntityId = 1;
	cubeModelConfiguration.materialEntityId = 7;
	cubeModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	cubeModelConfiguration.scale = shovelerVector3(1.0, 1.0, 1.0);
	cubeModelConfiguration.visible = true;
	cubeModelConfiguration.emitter = false;
	cubeModelConfiguration.screenspace = false;
	cubeModelConfiguration.castsShadow = true;
	cubeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *cubeEntity = shovelerViewAddEntity(view, 3);
	shovelerViewEntityAddModel(cubeEntity, cubeModelConfiguration);
	shovelerViewEntityAddPosition(cubeEntity, 0.0, 0.0, 5.0);

	ShovelerViewDrawableConfiguration quadDrawableConfiguration;
	quadDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_QUAD;
	ShovelerViewModelConfiguration planeModelConfiguration;
	planeModelConfiguration.drawableEntityId = 4;
	planeModelConfiguration.materialEntityId = 2;
	planeModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, 0.0);
	planeModelConfiguration.scale = shovelerVector3(10.0, 10.0, 1.0);
	planeModelConfiguration.visible = true;
	planeModelConfiguration.emitter = false;
	planeModelConfiguration.screenspace = false;
	planeModelConfiguration.castsShadow = true;
	planeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *planeEntity = shovelerViewAddEntity(view, 4);
	shovelerViewEntityAddDrawable(planeEntity, quadDrawableConfiguration);
	shovelerViewEntityAddModel(planeEntity, planeModelConfiguration);
	shovelerViewEntityAddPosition(planeEntity, 0.0, 0.0, 10.0);

	ShovelerViewDrawableConfiguration pointDrawableConfiguration;
	pointDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_POINT;
	ShovelerViewMaterialConfiguration whiteParticleMaterialConfiguration;
	whiteParticleMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE;
	whiteParticleMaterialConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewModelConfiguration lightModelConfiguration;
	lightModelConfiguration.drawableEntityId = 5;
	lightModelConfiguration.materialEntityId = 5;
	lightModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	lightModelConfiguration.scale = shovelerVector3(0.5, 0.5, 0.5);
	lightModelConfiguration.visible = true;
	lightModelConfiguration.emitter = true;
	lightModelConfiguration.screenspace = false;
	lightModelConfiguration.castsShadow = false;
	lightModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewLightConfiguration lightConfiguration;
	lightConfiguration.type = SHOVELER_VIEW_LIGHT_TYPE_POINT;
	lightConfiguration.width = 512;
	lightConfiguration.height = 512;
	lightConfiguration.samples = 1;
	lightConfiguration.ambientFactor = 0.0;
	lightConfiguration.exponentialFactor = 80.0;
	lightConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewEntity *lightEntity = shovelerViewAddEntity(view, 5);
	shovelerViewEntityAddDrawable(lightEntity, pointDrawableConfiguration);
	shovelerViewEntityAddMaterial(lightEntity, whiteParticleMaterialConfiguration);
	shovelerViewEntityAddModel(lightEntity, lightModelConfiguration);
	shovelerViewEntityAddLight(lightEntity, lightConfiguration);
	shovelerViewEntityAddPosition(lightEntity, 0.0, 2.0, 0.0);

	ShovelerImage *image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255;
	shovelerImageGet(image, 1, 1, 1) = 255;
	shovelerImageGet(image, 1, 1, 2) = 255;
	GString *imageData = getImageData(image);
	shovelerImageFree(image);
	ShovelerViewResourceConfiguration imageResourceConfiguration;
	imageResourceConfiguration.typeId = "image/png";
	imageResourceConfiguration.buffer = (unsigned char *) imageData->str;
	imageResourceConfiguration.bufferSize = imageData->len;
	ShovelerViewTextureConfiguration textureConfiguration;
	textureConfiguration.imageResourceEntityId = 6;
	textureConfiguration.interpolate = true;
	textureConfiguration.useMipmaps = true;
	textureConfiguration.clamp = true;
	ShovelerViewEntity *resourceEntity = shovelerViewAddEntity(view, 6);
	shovelerViewEntityAddResource(resourceEntity, imageResourceConfiguration);
	shovelerViewEntityAddTexture(resourceEntity, textureConfiguration);
	g_string_free(imageData, true);

	ShovelerViewMaterialConfiguration textureMaterialConfiguration;
	textureMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE;
	textureMaterialConfiguration.dataEntityId = 6;
	ShovelerViewEntity *textureMaterialEntity = shovelerViewAddEntity(view, 7);
	shovelerViewEntityAddMaterial(textureMaterialEntity, textureMaterialConfiguration);

	ShovelerViewTilemapTilesTileConfiguration layerTiles[4];
	layerTiles[0].tilesetColumn = 0;
	layerTiles[0].tilesetRow = 0;
	layerTiles[0].tilesetId = 1; // red
	layerTiles[1].tilesetColumn = 0;
	layerTiles[1].tilesetRow = 1;
	layerTiles[1].tilesetId = 1; // green
	layerTiles[2].tilesetColumn = 0;
	layerTiles[2].tilesetRow = 0;
	layerTiles[2].tilesetId = 1; // red
	layerTiles[3].tilesetColumn = 0;
	layerTiles[3].tilesetRow = 0;
	layerTiles[3].tilesetId = 2; // full tileset
	ShovelerViewTilemapTilesConfiguration layerConfiguration;
	layerConfiguration.isImageResourceEntityDefinition = false;
	layerConfiguration.imageResourceEntityId = 0;
	layerConfiguration.numColumns = 2;
	layerConfiguration.numRows = 2;
	layerConfiguration.tiles = layerTiles;
	ShovelerViewEntity *layerEntity = shovelerViewAddEntity(view, 8);
	shovelerViewEntityAddTilemapTiles(layerEntity, &layerConfiguration);

	ShovelerImage *layer2Image = shovelerImageCreate(3, 3, 3);
	shovelerImageClear(layer2Image);
	shovelerImageGet(layer2Image, 0, 0, 0) = 0;
	shovelerImageGet(layer2Image, 0, 0, 1) = 0;
	shovelerImageGet(layer2Image, 0, 0, 2) = 2; // full tileset
	GString *layer2ImageData = getImageData(layer2Image);
	shovelerImageFree(layer2Image);
	ShovelerViewResourceConfiguration layer2ImageResourceConfiguration;
	layer2ImageResourceConfiguration.typeId = "image/png";
	layer2ImageResourceConfiguration.buffer = (unsigned char *) layer2ImageData->str;
	layer2ImageResourceConfiguration.bufferSize = layer2ImageData->len;
	ShovelerViewTilemapTilesConfiguration layer2Configuration;
	layer2Configuration.isImageResourceEntityDefinition = true;
	layer2Configuration.imageResourceEntityId = 9;
	ShovelerViewEntity *layer2ResourceEntity = shovelerViewAddEntity(view, 9);
	shovelerViewEntityAddResource(layer2ResourceEntity, layer2ImageResourceConfiguration);
	shovelerViewEntityAddTilemapTiles(layer2ResourceEntity, &layer2Configuration);
	g_string_free(layer2ImageData, true);

	ShovelerViewTilesetConfiguration tilesetConfiguration;
	tilesetConfiguration.imageResourceEntityId = 6;
	tilesetConfiguration.columns = 2;
	tilesetConfiguration.rows = 2;
	tilesetConfiguration.padding = 1;
	ShovelerViewEntity *tilesetMaterialEntity = shovelerViewAddEntity(view, 10);
	shovelerViewEntityAddTileset(tilesetMaterialEntity, tilesetConfiguration);

	ShovelerViewTilesetConfiguration tileset2Configuration;
	tileset2Configuration.imageResourceEntityId = 6;
	tileset2Configuration.columns = 1;
	tileset2Configuration.rows = 1;
	tileset2Configuration.padding = 1;
	ShovelerViewEntity *tileset2MaterialEntity = shovelerViewAddEntity(view, 11);
	shovelerViewEntityAddTileset(tileset2MaterialEntity, tileset2Configuration);

	ShovelerViewTilemapConfiguration tilemapBackgroundConfiguration;
	tilemapBackgroundConfiguration.tilesEntityId = 8;
	tilemapBackgroundConfiguration.numTilesets = 2;
	tilemapBackgroundConfiguration.tilesetEntityIds = (long long int[]){10, 11};
	ShovelerViewEntity *tilemapBackgroundEntity = shovelerViewAddEntity(view, 12);
	shovelerViewEntityAddTilemap(tilemapBackgroundEntity, &tilemapBackgroundConfiguration);

	ShovelerViewTilemapConfiguration tilemapForegroundConfiguration;
	tilemapForegroundConfiguration.tilesEntityId = 9;
	tilemapForegroundConfiguration.numTilesets = 2;
	tilemapForegroundConfiguration.tilesetEntityIds = (long long int[]){10, 11};
	ShovelerViewEntity *tilemapForegroundEntity = shovelerViewAddEntity(view, 13);
	shovelerViewEntityAddTilemap(tilemapForegroundEntity, &tilemapForegroundConfiguration);

	ShovelerViewTileSpriteConfiguration tileSpriteConfiguration;
	tileSpriteConfiguration.tilesetEntityId = 11;
	tileSpriteConfiguration.tilesetColumn = 0;
	tileSpriteConfiguration.tilesetRow = 0;
	tileSpriteConfiguration.positionMappingX = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_X;
	tileSpriteConfiguration.positionMappingY = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Y;
	tileSpriteConfiguration.size = shovelerVector2(2.5, 4.0);
	ShovelerViewEntity *tileSpriteEntity = shovelerViewAddEntity(view, 14);
	shovelerViewEntityAddTileSprite(tileSpriteEntity, &tileSpriteConfiguration);
	shovelerViewEntityAddPosition(tileSpriteEntity, 3.0, 4.0, 0.0);

	ShovelerViewCanvasConfiguration canvasConfiguration;
	canvasConfiguration.numTileSprites = 1;
	canvasConfiguration.tileSpriteEntityIds = (long long int[]){14};
	ShovelerViewEntity *canvasEntity = shovelerViewAddEntity(view, 15);
	shovelerViewEntityAddCanvas(canvasEntity, &canvasConfiguration);

	ShovelerViewChunkConfiguration chunkConfiguration;
	chunkConfiguration.position = shovelerVector2(0.0f, 0.0f);
	chunkConfiguration.size = shovelerVector2(10.0f, 10.0f);
	chunkConfiguration.numLayers = 3;
	chunkConfiguration.layers = (ShovelerViewChunkLayerConfiguration[]){
		{SHOVELER_CHUNK_LAYER_TYPE_TILEMAP, 12},
		{SHOVELER_CHUNK_LAYER_TYPE_CANVAS, 15},
		{SHOVELER_CHUNK_LAYER_TYPE_TILEMAP, 13},
	};
	ShovelerViewEntity *chunkEntity = shovelerViewAddEntity(view, 16);
	shovelerViewEntityAddChunk(chunkEntity, &chunkConfiguration);

	ShovelerViewModelConfiguration tilemapMaterialModelConfiguration;
	tilemapMaterialModelConfiguration.drawableEntityId = 4;
	tilemapMaterialModelConfiguration.materialEntityId = 17;
	tilemapMaterialModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	tilemapMaterialModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	tilemapMaterialModelConfiguration.visible = true;
	tilemapMaterialModelConfiguration.emitter = false;
	tilemapMaterialModelConfiguration.screenspace = false;
	tilemapMaterialModelConfiguration.castsShadow = true;
	tilemapMaterialModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewMaterialConfiguration tilemapMaterialConfiguration;
	tilemapMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP;
	tilemapMaterialConfiguration.dataEntityId = 12;
	ShovelerViewEntity *tilemapMaterialEntity = shovelerViewAddEntity(view, 17);
	shovelerViewEntityAddModel(tilemapMaterialEntity, tilemapMaterialModelConfiguration);
	shovelerViewEntityAddMaterial(tilemapMaterialEntity, tilemapMaterialConfiguration);
	shovelerViewEntityAddPosition(tilemapMaterialEntity, 5.0, 5.0, 7.5);

	ShovelerViewModelConfiguration canvasMaterialModelConfiguration;
	canvasMaterialModelConfiguration.drawableEntityId = 4;
	canvasMaterialModelConfiguration.materialEntityId = 18;
	canvasMaterialModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	canvasMaterialModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	canvasMaterialModelConfiguration.visible = true;
	canvasMaterialModelConfiguration.emitter = false;
	canvasMaterialModelConfiguration.screenspace = false;
	canvasMaterialModelConfiguration.castsShadow = true;
	canvasMaterialModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewMaterialConfiguration canvasMaterialConfiguration;
	canvasMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_CANVAS;
	canvasMaterialConfiguration.canvasRegionPosition = shovelerVector2(5.0f, 5.0f);
	canvasMaterialConfiguration.canvasRegionSize = shovelerVector2(10.0f, 10.0f);
	canvasMaterialConfiguration.dataEntityId = 15;
	ShovelerViewEntity *canvasMaterialEntity = shovelerViewAddEntity(view, 18);
	shovelerViewEntityAddModel(canvasMaterialEntity, canvasMaterialModelConfiguration);
	shovelerViewEntityAddMaterial(canvasMaterialEntity, canvasMaterialConfiguration);
	shovelerViewEntityAddPosition(canvasMaterialEntity, 0.0, 5.0, 7.5);

	ShovelerViewModelConfiguration chunkMaterialModelConfiguration;
	chunkMaterialModelConfiguration.drawableEntityId = 4;
	chunkMaterialModelConfiguration.materialEntityId = 19;
	chunkMaterialModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	chunkMaterialModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	chunkMaterialModelConfiguration.visible = true;
	chunkMaterialModelConfiguration.emitter = false;
	chunkMaterialModelConfiguration.screenspace = false;
	chunkMaterialModelConfiguration.castsShadow = true;
	chunkMaterialModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewMaterialConfiguration chunkMaterialConfiguration;
	chunkMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_CHUNK;
	chunkMaterialConfiguration.dataEntityId = 16;
	ShovelerViewEntity *chunkMaterialEntity = shovelerViewAddEntity(view, 19);
	shovelerViewEntityAddModel(chunkMaterialEntity, chunkMaterialModelConfiguration);
	shovelerViewEntityAddMaterial(chunkMaterialEntity, chunkMaterialConfiguration);
	shovelerViewEntityAddPosition(chunkMaterialEntity, -5.0, 5.0, 7.5);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerViewFree(view);
	shovelerResourcesFree(resources);
	shovelerCameraPerspectiveDetachController(game->camera);
	shovelerControllerFree(controller);
	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static GString *getImageData(ShovelerImage *image)
{
	const char *tempImageFilename = "temp.png";
	shovelerImagePngWriteFile(image, tempImageFilename);

	unsigned char *contents;
	size_t contentsSize;
	shovelerFileRead(tempImageFilename, &contents, &contentsSize);

	GString *data = g_string_new("");
	g_string_append_len(data, (gchar *) contents, contentsSize);
	free(contents);

	return data;
}

static void updateGame(ShovelerGame *game, double dt)
{
	shovelerControllerUpdate(shovelerCameraPerspectiveGetController(game->camera), dt);
	shovelerCameraUpdateView(game->camera);

	time += dt;
	ShovelerViewEntity *lightEntity = shovelerViewGetEntity(view, 5);
	shovelerViewEntityUpdatePosition(lightEntity, 2.0 * sin(time), 2.0 * cos(time), 0.0);

	ShovelerViewEntity *tileSpriteEntity = shovelerViewGetEntity(view, 14);
	shovelerViewEntityUpdatePosition(tileSpriteEntity, 3.0 + 2.0 * sin(time), 4.0 + 2.0 * cos(time), 0.0);

	ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(view, 8);
	const ShovelerViewTilemapTilesConfiguration *layerConfiguration = shovelerViewEntityGetTilemapTilesConfiguration(tilemapEntity);
	layerConfiguration->tiles[0].tilesetColumn = (unsigned char) ((int) time % 2);
	shovelerViewEntityUpdateTilemapTilesData(tilemapEntity, layerConfiguration->tiles);
}
