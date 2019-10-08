#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memcpy

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/image/png.h>
#include <shoveler/resources/image_png.h>
#include <shoveler/view/canvas.h>
#include <shoveler/view/chunk.h>
#include <shoveler/view/chunk_layer.h>
#include <shoveler/view/client.h>
#include <shoveler/view/controller.h>
#include <shoveler/view/drawable.h>
#include <shoveler/view/light.h>
#include <shoveler/view/material.h>
#include <shoveler/view/model.h>
#include <shoveler/view/position.h>
#include <shoveler/view/resources.h>
#include <shoveler/view/sampler.h>
#include <shoveler/view/texture.h>
#include <shoveler/view/tile_sprite.h>
#include <shoveler/view/tile_sprite_animation.h>
#include <shoveler/view/tilemap.h>
#include <shoveler/view/tilemap_colliders.h>
#include <shoveler/view/tilemap_tiles.h>
#include <shoveler/view/tileset.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/file.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/opengl.h>
#include <shoveler/projection.h>
#include <shoveler/resources.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

static double time = 0.0;

static GString *getImageData(ShovelerImage *image);
static void updateGame(ShovelerGame *game, double dt);

int main(int argc, char *argv[])
{
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
	cameraSettings.projection.aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
	cameraSettings.projection.nearClippingPlane = 0.01;
	cameraSettings.projection.farClippingPlane = 1000;

	ShovelerGameControllerSettings controllerSettings;
	controllerSettings.frame = cameraSettings.frame;
	controllerSettings.moveFactor = 2.0f;
	controllerSettings.tiltFactor = 0.0005f;
	controllerSettings.boundingBoxSize2 = 0.0f;
	controllerSettings.boundingBoxSize3 = 0.0f;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerGame *game = shovelerGameCreate(updateGame, &windowSettings, &cameraSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	ShovelerResources *resources = shovelerResourcesCreate(NULL, NULL);
	shovelerResourcesImagePngRegister(resources);
	shovelerViewSetResources(game->view, resources);

	ShovelerViewDrawableConfiguration cubeDrawableConfiguration;
	cubeDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_CUBE;
	ShovelerViewEntity *cubeDrawableEntity = shovelerViewAddEntity(game->view, 1);
	shovelerViewEntityAddDrawable(cubeDrawableEntity, &cubeDrawableConfiguration);

	ShovelerViewMaterialConfiguration grayColorMaterialConfiguration;
	grayColorMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_COLOR;
	grayColorMaterialConfiguration.color = shovelerVector3(0.7, 0.7, 0.7);
	ShovelerViewEntity *grayColorMaterialEntity = shovelerViewAddEntity(game->view, 2);
	shovelerViewEntityAddMaterial(grayColorMaterialEntity, &grayColorMaterialConfiguration);

	ShovelerViewModelConfiguration cubeModelConfiguration;
	cubeModelConfiguration.positionEntityId = 3;
	cubeModelConfiguration.drawableEntityId = 1;
	cubeModelConfiguration.materialEntityId = 7;
	cubeModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	cubeModelConfiguration.scale = shovelerVector3(1.0, 1.0, 1.0);
	cubeModelConfiguration.visible = true;
	cubeModelConfiguration.emitter = false;
	cubeModelConfiguration.castsShadow = true;
	cubeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *cubeEntity = shovelerViewAddEntity(game->view, 3);
	shovelerViewEntityAddModel(cubeEntity, &cubeModelConfiguration);
	shovelerViewEntityAddPosition(cubeEntity, shovelerVector3(0.0, 0.0, 5.0));

	ShovelerViewDrawableConfiguration quadDrawableConfiguration;
	quadDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_QUAD;
	ShovelerViewModelConfiguration planeModelConfiguration;
	planeModelConfiguration.positionEntityId = 4;
	planeModelConfiguration.drawableEntityId = 4;
	planeModelConfiguration.materialEntityId = 2;
	planeModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, 0.0);
	planeModelConfiguration.scale = shovelerVector3(10.0, 10.0, 1.0);
	planeModelConfiguration.visible = true;
	planeModelConfiguration.emitter = false;
	planeModelConfiguration.castsShadow = true;
	planeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *planeEntity = shovelerViewAddEntity(game->view, 4);
	shovelerViewEntityAddDrawable(planeEntity, &quadDrawableConfiguration);
	shovelerViewEntityAddModel(planeEntity, &planeModelConfiguration);
	shovelerViewEntityAddPosition(planeEntity, shovelerVector3(0.0, 0.0, 10.0));

	ShovelerViewDrawableConfiguration pointDrawableConfiguration;
	pointDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_POINT;
	ShovelerViewMaterialConfiguration whiteParticleMaterialConfiguration;
	whiteParticleMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE;
	whiteParticleMaterialConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewModelConfiguration lightModelConfiguration;
	lightModelConfiguration.positionEntityId = 5;
	lightModelConfiguration.drawableEntityId = 5;
	lightModelConfiguration.materialEntityId = 5;
	lightModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	lightModelConfiguration.scale = shovelerVector3(0.5, 0.5, 0.5);
	lightModelConfiguration.visible = true;
	lightModelConfiguration.emitter = true;
	lightModelConfiguration.castsShadow = false;
	lightModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewLightConfiguration lightConfiguration;
	lightConfiguration.positionEntityId = 5;
	lightConfiguration.type = SHOVELER_VIEW_LIGHT_TYPE_POINT;
	lightConfiguration.width = 512;
	lightConfiguration.height = 512;
	lightConfiguration.samples = 1;
	lightConfiguration.ambientFactor = 0.0;
	lightConfiguration.exponentialFactor = 80.0;
	lightConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewEntity *lightEntity = shovelerViewAddEntity(game->view, 5);
	shovelerViewEntityAddDrawable(lightEntity, &pointDrawableConfiguration);
	shovelerViewEntityAddMaterial(lightEntity, &whiteParticleMaterialConfiguration);
	shovelerViewEntityAddModel(lightEntity, &lightModelConfiguration);
	shovelerViewEntityAddLight(lightEntity, &lightConfiguration);
	shovelerViewEntityAddPosition(lightEntity, shovelerVector3(0.0, 2.0, 0.0));

	ShovelerImage *image = shovelerImageCreate(2, 2, 4);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255; // red
	shovelerImageGet(image, 0, 0, 3) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255; // green
	shovelerImageGet(image, 0, 1, 3) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255; // blue
	shovelerImageGet(image, 1, 0, 3) = 255;
	shovelerImageGet(image, 1, 1, 0) = 255; // white
	shovelerImageGet(image, 1, 1, 1) = 255;
	shovelerImageGet(image, 1, 1, 2) = 255;
	shovelerImageGet(image, 1, 1, 3) = 255;
	GString *imageData = getImageData(image);
	ShovelerViewResourceConfiguration imageResourceConfiguration;
	imageResourceConfiguration.typeId = "image/png";
	imageResourceConfiguration.buffer = (unsigned char *) imageData->str;
	imageResourceConfiguration.bufferSize = imageData->len;
	ShovelerViewTextureConfiguration textureConfiguration;
	textureConfiguration.imageResourceEntityId = 6;
	ShovelerViewSamplerConfiguration samplerConfiguration;
	samplerConfiguration.interpolate = true;
	samplerConfiguration.useMipmaps = true;
	samplerConfiguration.clamp = true;
	ShovelerViewEntity *resourceEntity = shovelerViewAddEntity(game->view, 6);
	shovelerViewEntityAddResource(resourceEntity, &imageResourceConfiguration);
	shovelerViewEntityAddTexture(resourceEntity, &textureConfiguration);
	shovelerViewEntityAddSampler(resourceEntity, &samplerConfiguration);
	g_string_free(imageData, true);

	ShovelerViewMaterialConfiguration textureMaterialConfiguration;
	textureMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE;
	textureMaterialConfiguration.textureEntityId = 6;
	textureMaterialConfiguration.textureSamplerEntityId = 6;
	ShovelerViewEntity *textureMaterialEntity = shovelerViewAddEntity(game->view, 7);
	shovelerViewEntityAddMaterial(textureMaterialEntity, &textureMaterialConfiguration);

	unsigned char tilesetColumns[4];
	unsigned char tilesetRows[4];
	unsigned char tilesetIds[4];
	tilesetColumns[0] = 0;
	tilesetRows[0] = 0;
	tilesetIds[0] = 1; // red
	tilesetColumns[1] = 0;
	tilesetRows[1] = 1;
	tilesetIds[1] = 1; // green
	tilesetColumns[2] = 0;
	tilesetRows[2] = 0;
	tilesetIds[2] = 1; // red
	tilesetColumns[3] = 0;
	tilesetRows[3] = 0;
	tilesetIds[3] = 2; // full tileset
	ShovelerViewTilemapTilesConfiguration tilesConfiguration;
	tilesConfiguration.isImageResourceEntityDefinition = false;
	tilesConfiguration.imageResourceEntityId = 0;
	tilesConfiguration.numColumns = 2;
	tilesConfiguration.numRows = 2;
	tilesConfiguration.tilesetColumns = tilesetColumns;
	tilesConfiguration.tilesetRows = tilesetRows;
	tilesConfiguration.tilesetIds = tilesetIds;
	ShovelerViewTilemapCollidersConfiguration collidersConfiguration;
	collidersConfiguration.numColumns = 2;
	collidersConfiguration.numRows = 2;
	collidersConfiguration.colliders = (unsigned char[]){false, false, false, false};
	ShovelerViewEntity *layerEntity = shovelerViewAddEntity(game->view, 8);
	shovelerViewEntityAddTilemapTiles(layerEntity, &tilesConfiguration);
	shovelerViewEntityAddTilemapColliders(layerEntity, &collidersConfiguration);

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
	ShovelerViewTilemapCollidersConfiguration colliders2Configuration;
	colliders2Configuration.numColumns = 3;
	colliders2Configuration.numRows = 3;
	colliders2Configuration.colliders = (unsigned char[]){false, false, false, false, false, false, false, false, false};
	ShovelerViewEntity *layer2ResourceEntity = shovelerViewAddEntity(game->view, 9);
	shovelerViewEntityAddResource(layer2ResourceEntity, &layer2ImageResourceConfiguration);
	shovelerViewEntityAddTilemapTiles(layer2ResourceEntity, &layer2Configuration);
	g_string_free(layer2ImageData, true);
	shovelerViewEntityAddTilemapColliders(layer2ResourceEntity, &colliders2Configuration);

	ShovelerViewTilesetConfiguration tilesetConfiguration;
	tilesetConfiguration.imageResourceEntityId = 6;
	tilesetConfiguration.numColumns = 2;
	tilesetConfiguration.numRows = 2;
	tilesetConfiguration.padding = 1;
	ShovelerViewEntity *tilesetMaterialEntity = shovelerViewAddEntity(game->view, 10);
	shovelerViewEntityAddTileset(tilesetMaterialEntity, &tilesetConfiguration);

	ShovelerImage *animationTilesetImage = shovelerImageCreateAnimationTileset(image, 1);
	GString *animationTilesetImageData = getImageData(animationTilesetImage);
	ShovelerViewResourceConfiguration animationTilesetImageResourceConfiguration;
	animationTilesetImageResourceConfiguration.typeId = "image/png";
	animationTilesetImageResourceConfiguration.buffer = (unsigned char *) animationTilesetImageData->str;
	animationTilesetImageResourceConfiguration.bufferSize = animationTilesetImageData->len;
	ShovelerViewTextureConfiguration animationTilesetTextureConfiguration;
	animationTilesetTextureConfiguration.imageResourceEntityId = 11;
	ShovelerViewTilesetConfiguration animationTilesetConfiguration;
	animationTilesetConfiguration.imageResourceEntityId = 11;
	animationTilesetConfiguration.numColumns = 4;
	animationTilesetConfiguration.numRows = 3;
	animationTilesetConfiguration.padding = 1;
	ShovelerViewEntity *animationTilesetEntity = shovelerViewAddEntity(game->view, 11);
	shovelerViewEntityAddResource(animationTilesetEntity, &animationTilesetImageResourceConfiguration);
	shovelerViewEntityAddTexture(animationTilesetEntity, &animationTilesetTextureConfiguration);
	shovelerViewEntityAddTileset(animationTilesetEntity, &animationTilesetConfiguration);
	g_string_free(animationTilesetImageData, true);

	shovelerImageFree(image);
	shovelerImageFree(animationTilesetImage);

	ShovelerViewTilemapConfiguration tilemapBackgroundConfiguration;
	tilemapBackgroundConfiguration.tilesEntityId = 8;
	tilemapBackgroundConfiguration.collidersEntityId = 8;
	tilemapBackgroundConfiguration.numTilesets = 2;
	tilemapBackgroundConfiguration.tilesetEntityIds = (long long int[]){10, 11};
	ShovelerViewEntity *tilemapBackgroundEntity = shovelerViewAddEntity(game->view, 12);
	shovelerViewEntityAddTilemap(tilemapBackgroundEntity, &tilemapBackgroundConfiguration);

	ShovelerViewTilemapConfiguration tilemapForegroundConfiguration;
	tilemapForegroundConfiguration.tilesEntityId = 9;
	tilemapForegroundConfiguration.collidersEntityId = 9;
	tilemapForegroundConfiguration.numTilesets = 2;
	tilemapForegroundConfiguration.tilesetEntityIds = (long long int[]){10, 11};
	ShovelerViewEntity *tilemapForegroundEntity = shovelerViewAddEntity(game->view, 13);
	shovelerViewEntityAddTilemap(tilemapForegroundEntity, &tilemapForegroundConfiguration);

	ShovelerViewTileSpriteConfiguration tileSpriteConfiguration;
	tileSpriteConfiguration.positionEntityId = 14;
	tileSpriteConfiguration.tilesetEntityId = 11;
	tileSpriteConfiguration.tilesetColumn = 0;
	tileSpriteConfiguration.tilesetRow = 0;
	tileSpriteConfiguration.positionMappingX = SHOVELER_COORDINATE_MAPPING_POSITIVE_X;
	tileSpriteConfiguration.positionMappingY = SHOVELER_COORDINATE_MAPPING_POSITIVE_Y;
	tileSpriteConfiguration.size = shovelerVector2(2.5, 2.5);
	ShovelerViewTileSpriteAnimationConfiguration tileSpriteAnimationConfiguration;
	tileSpriteAnimationConfiguration.positionEntityId = 14;
	tileSpriteAnimationConfiguration.tileSpriteEntityId = 14;
	tileSpriteAnimationConfiguration.positionMappingX = SHOVELER_COORDINATE_MAPPING_POSITIVE_X;
	tileSpriteAnimationConfiguration.positionMappingY = SHOVELER_COORDINATE_MAPPING_POSITIVE_Y;
	tileSpriteAnimationConfiguration.moveAmountThreshold = 1.0f;
	ShovelerViewEntity *tileSpriteEntity = shovelerViewAddEntity(game->view, 14);
	shovelerViewEntityAddTileSprite(tileSpriteEntity, &tileSpriteConfiguration);
	shovelerViewEntityAddTileSpriteAnimation(tileSpriteEntity, &tileSpriteAnimationConfiguration);
	shovelerViewEntityAddPosition(tileSpriteEntity, shovelerVector3(3.0, 4.0, 0.0));

	ShovelerViewCanvasConfiguration canvasConfiguration;
	canvasConfiguration.numTileSprites = 1;
	canvasConfiguration.tileSpriteEntityIds = (long long int[]){14};
	ShovelerViewEntity *canvasEntity = shovelerViewAddEntity(game->view, 15);
	shovelerViewEntityAddCanvas(canvasEntity, &canvasConfiguration);

	ShovelerViewChunkLayerConfiguration chunkLayer1Configuration;
	chunkLayer1Configuration.type = SHOVELER_CHUNK_LAYER_TYPE_TILEMAP;
	chunkLayer1Configuration.tilemapEntityId = 12;
	ShovelerViewEntity *chunkLayer1Entity = shovelerViewAddEntity(game->view, 16);
	shovelerViewEntityAddChunkLayer(chunkLayer1Entity, &chunkLayer1Configuration);

	ShovelerViewChunkLayerConfiguration chunkLayer2Configuration;
	chunkLayer2Configuration.type = SHOVELER_CHUNK_LAYER_TYPE_TILEMAP;
	chunkLayer2Configuration.canvasEntityId = 15;
	ShovelerViewEntity *chunkLayer2Entity = shovelerViewAddEntity(game->view, 17);
	shovelerViewEntityAddChunkLayer(chunkLayer2Entity, &chunkLayer2Configuration);

	ShovelerViewChunkLayerConfiguration chunkLayer3Configuration;
	chunkLayer3Configuration.type = SHOVELER_CHUNK_LAYER_TYPE_TILEMAP;
	chunkLayer3Configuration.tilemapEntityId = 13;
	ShovelerViewEntity *chunkLayer3Entity = shovelerViewAddEntity(game->view, 18);
	shovelerViewEntityAddChunkLayer(chunkLayer3Entity, &chunkLayer3Configuration);

	ShovelerViewChunkConfiguration chunkConfiguration;
	chunkConfiguration.positionEntityId = 19;
	chunkConfiguration.positionMappingX = SHOVELER_COORDINATE_MAPPING_POSITIVE_X;
	chunkConfiguration.positionMappingY = SHOVELER_COORDINATE_MAPPING_POSITIVE_Y;
	chunkConfiguration.size = shovelerVector2(10.0f, 10.0f);
	chunkConfiguration.numLayers = 3;
	chunkConfiguration.layerEntityIds = (long long int[]){16, 17, 18};
	ShovelerViewEntity *chunkEntity = shovelerViewAddEntity(game->view, 19);
	shovelerViewEntityAddChunk(chunkEntity, &chunkConfiguration);
	shovelerViewEntityAddPosition(chunkEntity, shovelerVector3(5.0, 5.0, 0.0));

	ShovelerViewModelConfiguration tilemapMaterialModelConfiguration;
	tilemapMaterialModelConfiguration.positionEntityId = 20;
	tilemapMaterialModelConfiguration.drawableEntityId = 4;
	tilemapMaterialModelConfiguration.materialEntityId = 20;
	tilemapMaterialModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	tilemapMaterialModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	tilemapMaterialModelConfiguration.visible = true;
	tilemapMaterialModelConfiguration.emitter = false;
	tilemapMaterialModelConfiguration.castsShadow = true;
	tilemapMaterialModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewMaterialConfiguration tilemapMaterialConfiguration;
	tilemapMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP;
	tilemapMaterialConfiguration.tilemapEntityId = 12;
	ShovelerViewEntity *tilemapMaterialEntity = shovelerViewAddEntity(game->view, 20);
	shovelerViewEntitySetType(tilemapMaterialEntity, "tilemap");
	shovelerViewEntityAddModel(tilemapMaterialEntity, &tilemapMaterialModelConfiguration);
	shovelerViewEntityAddMaterial(tilemapMaterialEntity, &tilemapMaterialConfiguration);
	shovelerViewEntityAddPosition(tilemapMaterialEntity, shovelerVector3(5.0, 5.0, 7.5));

	ShovelerViewModelConfiguration canvasMaterialModelConfiguration;
	canvasMaterialModelConfiguration.positionEntityId = 21;
	canvasMaterialModelConfiguration.drawableEntityId = 4;
	canvasMaterialModelConfiguration.materialEntityId = 21;
	canvasMaterialModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	canvasMaterialModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	canvasMaterialModelConfiguration.visible = true;
	canvasMaterialModelConfiguration.emitter = false;
	canvasMaterialModelConfiguration.castsShadow = true;
	canvasMaterialModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewMaterialConfiguration canvasMaterialConfiguration;
	canvasMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_CANVAS;
	canvasMaterialConfiguration.canvasRegionPosition = shovelerVector2(5.0f, 5.0f);
	canvasMaterialConfiguration.canvasRegionSize = shovelerVector2(10.0f, 10.0f);
	canvasMaterialConfiguration.canvasEntityId = 15;
	ShovelerViewEntity *canvasMaterialEntity = shovelerViewAddEntity(game->view, 21);
	shovelerViewEntitySetType(canvasMaterialEntity, "canvas");
	shovelerViewEntityAddModel(canvasMaterialEntity, &canvasMaterialModelConfiguration);
	shovelerViewEntityAddMaterial(canvasMaterialEntity, &canvasMaterialConfiguration);
	shovelerViewEntityAddPosition(canvasMaterialEntity, shovelerVector3(0.0, 5.0, 7.5));

	ShovelerViewModelConfiguration chunkMaterialModelConfiguration;
	chunkMaterialModelConfiguration.positionEntityId = 22;
	chunkMaterialModelConfiguration.drawableEntityId = 4;
	chunkMaterialModelConfiguration.materialEntityId = 22;
	chunkMaterialModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	chunkMaterialModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	chunkMaterialModelConfiguration.visible = true;
	chunkMaterialModelConfiguration.emitter = false;
	chunkMaterialModelConfiguration.castsShadow = true;
	chunkMaterialModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewMaterialConfiguration chunkMaterialConfiguration;
	chunkMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_CHUNK;
	chunkMaterialConfiguration.chunkEntityId = 16;
	ShovelerViewEntity *chunkMaterialEntity = shovelerViewAddEntity(game->view, 22);
	shovelerViewEntitySetType(chunkMaterialEntity, "chunk");
	shovelerViewEntityAddModel(chunkMaterialEntity, &chunkMaterialModelConfiguration);
	shovelerViewEntityAddMaterial(chunkMaterialEntity, &chunkMaterialConfiguration);
	shovelerViewEntityAddPosition(chunkMaterialEntity, shovelerVector3(-5.0, 5.0, 7.5));

	ShovelerViewClientConfiguration clientConfiguration;
	clientConfiguration.positionEntityId = 23;
	clientConfiguration.modelEntityId = 0;
	ShovelerViewEntity *clientEntity = shovelerViewAddEntity(game->view, 23);
	shovelerViewEntitySetType(clientEntity, "client");
	shovelerViewEntityAddPosition(clientEntity, shovelerVector3(0.0, 0.0, 0.0));
	shovelerViewEntityAddClient(clientEntity, &clientConfiguration);
	shovelerViewEntityDelegate(clientEntity, shovelerViewPositionComponentTypeName);
	shovelerViewEntityDelegate(clientEntity, shovelerViewClientComponentTypeName);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerGameFree(game);
	shovelerResourcesFree(resources);
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
	shovelerCameraUpdateView(game->camera);

	time += dt;
	ShovelerViewEntity *lightEntity = shovelerViewGetEntity(game->view, 5);
	ShovelerComponent *lightPositionComponent = shovelerViewEntityGetPositionComponent(lightEntity);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(lightPositionComponent, shovelerViewPositionCoordinatesOptionKey, shovelerVector3(2.0 * sin(time), 2.0 * cos(time), 0.0));

	ShovelerViewEntity *tileSpriteEntity = shovelerViewGetEntity(game->view, 14);
	ShovelerComponent *tileSpritePositionComponent = shovelerViewEntityGetPositionComponent(tileSpriteEntity);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(tileSpritePositionComponent, shovelerViewPositionCoordinatesOptionKey, shovelerVector3(3.0 + 2.0 * sin(time), 4.0 + 2.0 * cos(time), 0.0));

	ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(game->view, 8);
	ShovelerComponent *tilemapComponent = shovelerViewEntityGetTilemapTilesComponent(tilemapEntity);
	const unsigned char *tilesetColumns;
	shovelerComponentGetConfigurationValueBytes(tilemapComponent, shovelerViewTilemapTilesTilesetColumnsOptionKey, &tilesetColumns, /* outputSize */ NULL);
	unsigned char updatedTilesetColumns[4];
	memcpy(&updatedTilesetColumns, tilesetColumns, 4 * sizeof(unsigned char));
	updatedTilesetColumns[0] = (unsigned char) ((int) time % 2);

	shovelerComponentUpdateCanonicalConfigurationOptionBytes(tilemapComponent, shovelerViewTilemapTilesTilesetColumnsOptionKey, updatedTilesetColumns, 4);
}
