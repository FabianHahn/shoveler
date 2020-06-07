#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memcpy

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/image/png.h>
#include <shoveler/resources/image_png.h>
#include <shoveler/component/canvas.h>
#include <shoveler/component/client.h>
#include <shoveler/component/controller.h>
#include <shoveler/component/drawable.h>
#include <shoveler/component/image.h>
#include <shoveler/component/light.h>
#include <shoveler/component/material.h>
#include <shoveler/component/model.h>
#include <shoveler/component/position.h>
#include <shoveler/component/resource.h>
#include <shoveler/component/resources.h>
#include <shoveler/component/sampler.h>
#include <shoveler/component/sprite.h>
#include <shoveler/component/texture.h>
#include <shoveler/component/tile_sprite.h>
#include <shoveler/component/tile_sprite_animation.h>
#include <shoveler/component/tilemap.h>
#include <shoveler/component/tilemap_colliders.h>
#include <shoveler/component/tilemap_sprite.h>
#include <shoveler/component/tilemap_tiles.h>
#include <shoveler/component/tileset.h>
#include <shoveler/component.h>
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
static void updateAuthoritativeViewComponent(ShovelerGame *game, ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *unused);

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

	ShovelerGame *game = shovelerGameCreate(updateGame, updateAuthoritativeViewComponent, /* updateAuthoritativeViewComponentUserData */ NULL, &windowSettings, &cameraSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	ShovelerResources *resources = shovelerResourcesCreate(NULL, NULL);
	shovelerResourcesImagePngRegister(resources);
	shovelerViewSetTarget(game->view, shovelerComponentViewTargetIdResources, resources);

	ShovelerViewEntity *cubeDrawableEntity = shovelerViewAddEntity(game->view, 1);
	ShovelerComponent *cubeDrawableComponent = shovelerViewEntityAddComponent(cubeDrawableEntity, shovelerComponentTypeIdDrawable);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(cubeDrawableComponent, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE, SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE);
	shovelerComponentActivate(cubeDrawableComponent);

	ShovelerViewEntity *grayColorMaterialEntity = shovelerViewAddEntity(game->view, 2);
	ShovelerComponent *grayColorMaterialComponent = shovelerViewEntityAddComponent(grayColorMaterialEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(grayColorMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR);
	shovelerComponentUpdateCanonicalConfigurationOptionVector4(grayColorMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, shovelerVector4(0.7f, 0.7f, 0.7f, 1.0f));
	shovelerComponentActivate(grayColorMaterialComponent);

	ShovelerViewEntity *cubeEntity = shovelerViewAddEntity(game->view, 3);
	ShovelerComponent *cubeComponent = shovelerViewEntityAddComponent(cubeEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 3);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 1);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 7);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(0.0f, 0.0f, 0.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(1.0f, 1.0f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, false);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(cubeComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(grayColorMaterialComponent);
	ShovelerComponent *cubePositionComponent = shovelerViewEntityAddComponent(cubeEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(cubePositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(0.0f, 0.0f, 5.0f));
	shovelerComponentActivate(cubePositionComponent);

	ShovelerViewEntity *planeEntity = shovelerViewAddEntity(game->view, 4);
	ShovelerComponent *quadDrawableComponent = shovelerViewEntityAddComponent(planeEntity, shovelerComponentTypeIdDrawable);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(quadDrawableComponent, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE, SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD);
	shovelerComponentActivate(quadDrawableComponent);
	ShovelerComponent *planeModelComponent = shovelerViewEntityAddComponent(planeEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(SHOVELER_PI, 0.0f, 0.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(10.0f, 10.0f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, false);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(planeModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(planeModelComponent);
	ShovelerComponent *planePositionComponent = shovelerViewEntityAddComponent(planeEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(planePositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(0.0f, 0.0f, 10.0f));
	shovelerComponentActivate(planePositionComponent);

	ShovelerViewEntity *pointEntity = shovelerViewAddEntity(game->view, 5);
	ShovelerComponent *pointDrawableComponent = shovelerViewEntityAddComponent(pointEntity, shovelerComponentTypeIdDrawable);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(pointDrawableComponent, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE, SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT);
	shovelerComponentActivate(pointDrawableComponent);
	ShovelerComponent *whiteParticleMaterialComponent = shovelerViewEntityAddComponent(pointEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(whiteParticleMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE);
	shovelerComponentUpdateCanonicalConfigurationOptionVector4(whiteParticleMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, shovelerVector4(1.0f, 1.0f, 1.0f, 1.0f));
	shovelerComponentActivate(whiteParticleMaterialComponent);
	ShovelerComponent *lightModelComponent = shovelerViewEntityAddComponent(pointEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 5);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 5);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 5);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(0.0f, 0.0f, 0.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(0.5f, 0.5f, 0.5f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(lightModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(lightModelComponent);
	ShovelerComponent *lightComponent = shovelerViewEntityAddComponent(pointEntity, shovelerComponentTypeIdLight);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION, 5);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE, SHOVELER_COMPONENT_LIGHT_TYPE_POINT);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH, 512);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT, 512);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES, 1);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR, 0.0f);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR, 80.0f);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(lightComponent, SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR, shovelerVector3(1.0f, 1.0f, 1.0));
	shovelerComponentActivate(lightComponent);
	ShovelerComponent *lightPositionComponent = shovelerViewEntityAddComponent(pointEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(lightPositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(0.0f, 2.0f, 0.0f));
	shovelerComponentActivate(lightPositionComponent);

	ShovelerViewEntity *resourceEntity = shovelerViewAddEntity(game->view, 6);
	ShovelerComponent *resourceComponent = shovelerViewEntityAddComponent(resourceEntity, shovelerComponentTypeIdResource);
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
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(resourceComponent, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, (const unsigned char *) imageData->str, (int) imageData->len);
	g_string_free(imageData, true);
	shovelerComponentActivate(resourceComponent);
	ShovelerComponent *resourceImageComponent = shovelerViewEntityAddComponent(resourceEntity, shovelerComponentTypeIdImage);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(resourceImageComponent, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(resourceImageComponent, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE, 6);
	shovelerComponentActivate(resourceImageComponent);
	ShovelerComponent *resourceTextureComponent = shovelerViewEntityAddComponent(resourceEntity, shovelerComponentTypeIdTexture);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(resourceTextureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TYPE, SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(resourceTextureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE, 6);
	shovelerComponentActivate(resourceTextureComponent);
	ShovelerComponent *resourceSamplerComponent = shovelerViewEntityAddComponent(resourceEntity, shovelerComponentTypeIdSampler);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(resourceSamplerComponent, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(resourceSamplerComponent, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(resourceSamplerComponent, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP, true);
	shovelerComponentActivate(resourceSamplerComponent);

	ShovelerViewEntity *textureMaterialEntity = shovelerViewAddEntity(game->view, 7);
	ShovelerComponent *textureMaterialComponent = shovelerViewEntityAddComponent(textureMaterialEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(textureMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(textureMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_TYPE, SHOVELER_MATERIAL_TEXTURE_TYPE_PHONG);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(textureMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE, 6);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(textureMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER, 6);
	shovelerComponentActivate(textureMaterialComponent);

	ShovelerViewEntity *layerEntity = shovelerViewAddEntity(game->view, 8);
	ShovelerComponent *layerTilesComponent = shovelerViewEntityAddComponent(layerEntity, shovelerComponentTypeIdTilemapTiles);
	shovelerComponentClearConfigurationOption(layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, /* isCaononical */ true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS, 2);
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
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, tilesetColumns, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, tilesetRows, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, tilesetIds, 4);
	shovelerComponentActivate(layerTilesComponent);
	ShovelerComponent *layerCollidersComponent = shovelerViewEntityAddComponent(layerEntity, shovelerComponentTypeIdTilemapColliders);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layerCollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layerCollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, 2);
	unsigned char colliders[] = {false, false, false, false};
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(layerCollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, colliders, 4);
	shovelerComponentActivate(layerCollidersComponent);

	ShovelerViewEntity *layer2ResourceEntity = shovelerViewAddEntity(game->view, 9);
	ShovelerComponent *layer2ResourceComponent = shovelerViewEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdResource);
	ShovelerImage *layer2Image = shovelerImageCreate(3, 3, 3);
	shovelerImageClear(layer2Image);
	shovelerImageGet(layer2Image, 0, 0, 0) = 0;
	shovelerImageGet(layer2Image, 0, 0, 1) = 0;
	shovelerImageGet(layer2Image, 0, 0, 2) = 2; // full tileset
	GString *layer2ImageData = getImageData(layer2Image);
	shovelerImageFree(layer2Image);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(layer2ResourceComponent, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, (const unsigned char *) layer2ImageData->str, (int) layer2ImageData->len);
	g_string_free(layer2ImageData, true);
	shovelerComponentActivate(layer2ResourceComponent);
	ShovelerComponent *layer2ResourceImageComponent = shovelerViewEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdImage);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layer2ResourceImageComponent, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(layer2ResourceImageComponent, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE, 9);
	shovelerComponentActivate(layer2ResourceImageComponent);
	ShovelerComponent *layer2TilesComponent = shovelerViewEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdTilemapTiles);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(layer2TilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, 9);
	shovelerComponentActivate(layer2TilesComponent);
	ShovelerComponent *layer2CollidersComponent = shovelerViewEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdTilemapColliders);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layer2CollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, 3);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(layer2CollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, 3);
	unsigned char colliders2[] = {false, false, false, false, false, false, false, false, false};
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(layer2CollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, colliders2, 9);
	shovelerComponentActivate(layer2CollidersComponent);

	ShovelerViewEntity *tilesetMaterialEntity = shovelerViewAddEntity(game->view, 10);
	ShovelerComponent *tilesetComponent = shovelerViewEntityAddComponent(tilesetMaterialEntity, shovelerComponentTypeIdTileset);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE, 6);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING, 1);
	shovelerComponentActivate(tilesetComponent);

	ShovelerViewEntity *animationTilesetEntity = shovelerViewAddEntity(game->view, 11);
	ShovelerComponent *animationResourceComponent = shovelerViewEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdResource);
	ShovelerImage *animationTilesetImage = shovelerImageCreateAnimationTileset(image, 1);
	shovelerImageFree(image);
	GString *animationTilesetImageData = getImageData(animationTilesetImage);
	shovelerImageFree(animationTilesetImage);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(animationResourceComponent, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, (const unsigned char *) animationTilesetImageData->str, (int) animationTilesetImageData->len);
	g_string_free(animationTilesetImageData, true);
	shovelerComponentActivate(animationResourceComponent);
	ShovelerComponent *animationImageComponent = shovelerViewEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdImage);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(animationImageComponent, SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(animationImageComponent, SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE, 11);
	shovelerComponentActivate(animationImageComponent);
	ShovelerComponent *animationTextureComponent = shovelerViewEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdTexture);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(animationTextureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TYPE, SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(animationTextureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE, 11);
	shovelerComponentActivate(animationTextureComponent);
	ShovelerComponent *animationTilesetComponent = shovelerViewEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdTileset);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(animationTilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_IMAGE, 11);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(animationTilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_COLUMNS, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(animationTilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_NUM_ROWS, 3);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(animationTilesetComponent, SHOVELER_COMPONENT_TILESET_OPTION_ID_PADDING, 1);
	shovelerComponentActivate(animationTilesetComponent);

	ShovelerViewEntity *tilemapBackgroundEntity = shovelerViewAddEntity(game->view, 12);
	ShovelerComponent *tilemapBackgroundComponent = shovelerViewEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdTilemap);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES, 8);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS, 8);
	long long int tilesetEntityIds[] = {10, 11};
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(tilemapBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS, tilesetEntityIds, 2);
	shovelerComponentActivate(tilemapBackgroundComponent);
	ShovelerComponent *tilemapSpriteBackgroundComponent = shovelerViewEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdTilemapSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapSpriteBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_MATERIAL, 12);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapSpriteBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_TILEMAP, 12);
	shovelerComponentActivate(tilemapSpriteBackgroundComponent);
	ShovelerComponent *backgroundSpriteComponent = shovelerViewEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION, 12);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X, SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y, SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS, 16);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER, 0);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE, shovelerVector2(10.0f, 10.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILEMAP_SPRITE, 12);
	shovelerComponentActivate(backgroundSpriteComponent);
	ShovelerComponent *backgrouindTilemapMaterialComponent = shovelerViewEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(backgrouindTilemapMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP);
	shovelerComponentActivate(backgrouindTilemapMaterialComponent);
	ShovelerComponent *backgroundSpritePositionComponent = shovelerViewEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(backgroundSpritePositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(5.0f, 5.0f, 0.0f));
	shovelerComponentActivate(backgroundSpritePositionComponent);

	ShovelerViewEntity *tilemapForegroundEntity = shovelerViewAddEntity(game->view, 13);
	ShovelerComponent *tilemapForegroundComponent = shovelerViewEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdTilemap);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapForegroundComponent, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES, 9);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapForegroundComponent, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS, 9);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(tilemapForegroundComponent, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS, tilesetEntityIds, 2);
	shovelerComponentActivate(tilemapForegroundComponent);
	ShovelerComponent *tilemapSpriteForegroundComponent = shovelerViewEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdTilemapSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapSpriteForegroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_MATERIAL, 12);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapSpriteForegroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_TILEMAP, 13);
	shovelerComponentActivate(tilemapSpriteForegroundComponent);
	ShovelerComponent *foregroundSpriteComponent = shovelerViewEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION, 13);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X, SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y, SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS, 16);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE, shovelerVector2(10.0f, 10.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILEMAP_SPRITE, 13);
	shovelerComponentActivate(foregroundSpriteComponent);
	ShovelerComponent *foregroundSpritePositionComponent = shovelerViewEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(foregroundSpritePositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(5.0f, 5.0f, 0.0f));
	shovelerComponentActivate(foregroundSpritePositionComponent);

	ShovelerViewEntity *tileSpriteEntity = shovelerViewAddEntity(game->view, 14);
	ShovelerComponent *spriteComponent = shovelerViewEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION, 14);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X, SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y, SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS, 15);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER, 0);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE, shovelerVector2(2.5f, 2.5f));
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(spriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE, 14);
	shovelerComponentActivate(spriteComponent);
	ShovelerComponent *tileSpriteComponent = shovelerViewEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdTileSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_MATERIAL, 14);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET, 11);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_COLUMN, 0);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_ROW, 0);
	shovelerComponentActivate(tileSpriteComponent);
	ShovelerComponent *tileSpriteAnimationComponent = shovelerViewEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdTileSpriteAnimation);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tileSpriteAnimationComponent, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION, 14);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tileSpriteAnimationComponent, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE, 14);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tileSpriteAnimationComponent, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X, SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tileSpriteAnimationComponent, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y, SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(tileSpriteAnimationComponent, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD, 1.0f);
	shovelerComponentActivate(tileSpriteAnimationComponent);
	ShovelerComponent *tileSpriteMaterialComponent = shovelerViewEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tileSpriteMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_TILE_SPRITE);
	shovelerComponentActivate(tileSpriteMaterialComponent);
	ShovelerComponent *tileSpritePositionComponent = shovelerViewEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(tileSpritePositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(3.0f, 4.0f, 0.0f));
	shovelerComponentActivate(tileSpritePositionComponent);

	ShovelerViewEntity *canvasEntity = shovelerViewAddEntity(game->view, 15);
	ShovelerComponent *canvasComponent = shovelerViewEntityAddComponent(canvasEntity, shovelerComponentTypeIdCanvas);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(canvasComponent, SHOVELER_COMPONENT_CANVAS_OPTION_ID_NUM_LAYERS, 1);
	shovelerComponentActivate(canvasComponent);

	ShovelerViewEntity *chunkEntity = shovelerViewAddEntity(game->view, 16);
	ShovelerComponent *chunkCanvasComponent = shovelerViewEntityAddComponent(chunkEntity, shovelerComponentTypeIdCanvas);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(chunkCanvasComponent, SHOVELER_COMPONENT_CANVAS_OPTION_ID_NUM_LAYERS, 3);
	shovelerComponentActivate(chunkCanvasComponent);

	ShovelerViewEntity *chunkTileSpriteEntity = shovelerViewAddEntity(game->view, 17);
	ShovelerComponent *chunkSpriteComponent = shovelerViewEntityAddComponent(chunkTileSpriteEntity, shovelerComponentTypeIdSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION, 14);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X, SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y, SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS, 16);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER, 1);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE, shovelerVector2(2.5f, 2.5f));
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE, 14);
	shovelerComponentActivate(chunkSpriteComponent);

	ShovelerViewEntity *tilemapMaterialEntity = shovelerViewAddEntity(game->view, 20);
	ShovelerComponent *tilemapMaterialModelComponent = shovelerViewEntityAddComponent(tilemapMaterialEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 20);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 20);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(0.5f, 0.5f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, false);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(tilemapMaterialModelComponent);
	ShovelerComponent *tilemapMaterialComponent = shovelerViewEntityAddComponent(tilemapMaterialEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(tilemapMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(tilemapMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP, 12);
	shovelerComponentActivate(tilemapMaterialComponent);
	ShovelerComponent *tilemapMaterialPositionComponent = shovelerViewEntityAddComponent(tilemapMaterialEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(tilemapMaterialPositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(5.0f, 5.0f, 7.5f));
	shovelerComponentActivate(tilemapMaterialPositionComponent);

	ShovelerViewEntity *canvasMaterialEntity = shovelerViewAddEntity(game->view, 21);
	ShovelerComponent *canvasMaterialModelComponent = shovelerViewEntityAddComponent(canvasMaterialEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 21);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 21);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(0.5f, 0.5f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, false);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(canvasMaterialModelComponent);
	ShovelerComponent *canvasMaterialComponent = shovelerViewEntityAddComponent(canvasMaterialEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(canvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(canvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS, 15);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(canvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION, shovelerVector2(5.0f, 5.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(canvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE, shovelerVector2(10.0f, 10.0f));
	shovelerComponentActivate(canvasMaterialComponent);
	ShovelerComponent *canvasMaterialPositionComponent = shovelerViewEntityAddComponent(canvasMaterialEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(canvasMaterialPositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(0.0f, 5.0f, 7.5f));
	shovelerComponentActivate(canvasMaterialPositionComponent);

	ShovelerViewEntity *chunkMaterialEntity = shovelerViewAddEntity(game->view, 22);
	ShovelerComponent *chunkMaterialModelComponent = shovelerViewEntityAddComponent(chunkMaterialEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 22);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 4);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 22);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(0.5f, 0.5f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, false);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, true);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(chunkMaterialModelComponent);
	ShovelerComponent *chunkCanvasMaterialComponent = shovelerViewEntityAddComponent(chunkMaterialEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(chunkCanvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(chunkCanvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS, 16);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(chunkCanvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION, shovelerVector2(5.0f, 5.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(chunkCanvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE, shovelerVector2(10.0f, 10.0f));
	shovelerComponentActivate(chunkCanvasMaterialComponent);
	ShovelerComponent *chunkMaterialPositionComponent = shovelerViewEntityAddComponent(chunkMaterialEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(chunkMaterialPositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(-5.0f, 5.0f, 7.5f));
	shovelerComponentActivate(chunkMaterialPositionComponent);

	ShovelerViewEntity *clientEntity = shovelerViewAddEntity(game->view, 23);
	ShovelerComponent *clientComponent = shovelerViewEntityAddComponent(clientEntity, shovelerComponentTypeIdClient);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(clientComponent, SHOVELER_COMPONENT_CLIENT_OPTION_ID_POSITION, 23);
	shovelerViewEntityDelegate(clientEntity, shovelerComponentTypeIdClient);
	shovelerComponentActivate(clientComponent);
	ShovelerComponent *clientPositionComponent = shovelerViewEntityAddComponent(clientEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(clientPositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(0.0f, 0.0f, 0.0f));
	shovelerViewEntityDelegate(clientEntity, shovelerComponentTypeIdPosition);
	shovelerComponentActivate(clientPositionComponent);

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
	ShovelerComponent *lightPositionComponent = shovelerViewEntityGetComponent(lightEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(lightPositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(2.0 * sin(time), 2.0 * cos(time), 0.0));

	ShovelerViewEntity *tileSpriteEntity = shovelerViewGetEntity(game->view, 14);
	ShovelerComponent *tileSpritePositionComponent = shovelerViewEntityGetComponent(tileSpriteEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(tileSpritePositionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(3.0 + 2.0 * sin(time), 4.0 + 2.0 * cos(time), 0.0));

	ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(game->view, 8);
	ShovelerComponent *tilemapComponent = shovelerViewEntityGetComponent(tilemapEntity, shovelerComponentTypeIdTilemapTiles);
	const unsigned char *tilesetColumns;
	shovelerComponentGetConfigurationValueBytes(tilemapComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, &tilesetColumns, /* outputSize */ NULL);
	unsigned char updatedTilesetColumns[4];
	memcpy(&updatedTilesetColumns, tilesetColumns, 4 * sizeof(unsigned char));
	updatedTilesetColumns[0] = (unsigned char) ((int) time % 2);

	shovelerComponentUpdateCanonicalConfigurationOptionBytes(tilemapComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS, updatedTilesetColumns, 4);
}

static void updateAuthoritativeViewComponent(ShovelerGame *game, ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *unused)
{
	GString *printedValue = shovelerComponentConfigurationValuePrint(value);

	shovelerLogInfo("Updating configuration option '%s' of component '%s' on entity %lld: %s", configurationOption->name, component->type->id, component->entityId, printedValue->str);

	g_string_free(printedValue, true);
}
