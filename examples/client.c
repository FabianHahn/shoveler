#include <shoveler/camera/perspective.h>
#include <shoveler/client_system.h>
#include <shoveler/component.h>
#include <shoveler/component_field.h>
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
#include <shoveler/resources/image_png.h>
#include <shoveler/scene.h>
#include <shoveler/schema/base.h>
#include <shoveler/schema/opengl.h>
#include <shoveler/types.h>
#include <shoveler/world.h>
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memcpy

static double time = 0.0;

static GString* getImageData(ShovelerImage* image);
static void updateGame(ShovelerGame* game, double dt);
static void updateAuthoritativeComponent(
    ShovelerClientSystem* clientSystem,
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData);

static ShovelerWorld* world = NULL;

int main(int argc, char* argv[]) {
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

  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
  shovelerGlobalInit();

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

  ShovelerWorldEntity* cubeDrawableEntity = shovelerWorldAddEntity(world, 1);
  ShovelerComponent* cubeDrawableComponent =
      shovelerWorldEntityAddComponent(cubeDrawableEntity, shovelerComponentTypeIdDrawable);
  shovelerComponentUpdateCanonicalFieldInt(
      cubeDrawableComponent,
      SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE);
  shovelerComponentActivate(cubeDrawableComponent);

  ShovelerWorldEntity* grayColorMaterialEntity = shovelerWorldAddEntity(world, 2);
  ShovelerComponent* grayColorMaterialComponent =
      shovelerWorldEntityAddComponent(grayColorMaterialEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      grayColorMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR);
  shovelerComponentUpdateCanonicalFieldVector4(
      grayColorMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR,
      shovelerVector4(0.7f, 0.7f, 0.7f, 1.0f));
  shovelerComponentActivate(grayColorMaterialComponent);

  ShovelerWorldEntity* cubeEntity = shovelerWorldAddEntity(world, 3);
  ShovelerComponent* cubeComponent =
      shovelerWorldEntityAddComponent(cubeEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 3);
  shovelerComponentUpdateCanonicalFieldEntityId(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 1);
  shovelerComponentUpdateCanonicalFieldEntityId(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 7);
  shovelerComponentUpdateCanonicalFieldVector3(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION, shovelerVector3(0.0f, 0.0f, 0.0f));
  shovelerComponentUpdateCanonicalFieldVector3(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE, shovelerVector3(1.0f, 1.0f, 1.0f));
  shovelerComponentUpdateCanonicalFieldBool(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, false);
  shovelerComponentUpdateCanonicalFieldBool(
      cubeComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, true);
  shovelerComponentUpdateCanonicalFieldInt(
      cubeComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(grayColorMaterialComponent);
  ShovelerComponent* cubePositionComponent =
      shovelerWorldEntityAddComponent(cubeEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      cubePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      cubePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(0.0f, 0.0f, 5.0f));
  shovelerComponentActivate(cubePositionComponent);

  ShovelerWorldEntity* planeEntity = shovelerWorldAddEntity(world, 4);
  ShovelerComponent* quadDrawableComponent =
      shovelerWorldEntityAddComponent(planeEntity, shovelerComponentTypeIdDrawable);
  shovelerComponentUpdateCanonicalFieldInt(
      quadDrawableComponent,
      SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD);
  shovelerComponentActivate(quadDrawableComponent);
  ShovelerComponent* planeModelComponent =
      shovelerWorldEntityAddComponent(planeEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      planeModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 4);
  shovelerComponentUpdateCanonicalFieldEntityId(
      planeModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 4);
  shovelerComponentUpdateCanonicalFieldEntityId(
      planeModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 2);
  shovelerComponentUpdateCanonicalFieldVector3(
      planeModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION,
      shovelerVector3(SHOVELER_PI, 0.0f, 0.0f));
  shovelerComponentUpdateCanonicalFieldVector3(
      planeModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE,
      shovelerVector3(10.0f, 10.0f, 1.0f));
  shovelerComponentUpdateCanonicalFieldBool(
      planeModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      planeModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, false);
  shovelerComponentUpdateCanonicalFieldBool(
      planeModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, true);
  shovelerComponentUpdateCanonicalFieldInt(
      planeModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(planeModelComponent);
  ShovelerComponent* planePositionComponent =
      shovelerWorldEntityAddComponent(planeEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      planePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      planePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(0.0f, 0.0f, 10.0f));
  shovelerComponentActivate(planePositionComponent);

  ShovelerWorldEntity* pointEntity = shovelerWorldAddEntity(world, 5);
  ShovelerComponent* pointDrawableComponent =
      shovelerWorldEntityAddComponent(pointEntity, shovelerComponentTypeIdDrawable);
  shovelerComponentUpdateCanonicalFieldInt(
      pointDrawableComponent,
      SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT);
  shovelerComponentActivate(pointDrawableComponent);
  ShovelerComponent* whiteParticleMaterialComponent =
      shovelerWorldEntityAddComponent(pointEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      whiteParticleMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE);
  shovelerComponentUpdateCanonicalFieldVector4(
      whiteParticleMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR,
      shovelerVector4(1.0f, 1.0f, 1.0f, 1.0f));
  shovelerComponentActivate(whiteParticleMaterialComponent);
  ShovelerComponent* lightModelComponent =
      shovelerWorldEntityAddComponent(pointEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      lightModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 5);
  shovelerComponentUpdateCanonicalFieldEntityId(
      lightModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 5);
  shovelerComponentUpdateCanonicalFieldEntityId(
      lightModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 5);
  shovelerComponentUpdateCanonicalFieldVector3(
      lightModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION,
      shovelerVector3(0.0f, 0.0f, 0.0f));
  shovelerComponentUpdateCanonicalFieldVector3(
      lightModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE,
      shovelerVector3(0.5f, 0.5f, 0.5f));
  shovelerComponentUpdateCanonicalFieldBool(
      lightModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      lightModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, true);
  shovelerComponentUpdateCanonicalFieldBool(
      lightModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, true);
  shovelerComponentUpdateCanonicalFieldInt(
      lightModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(lightModelComponent);
  ShovelerComponent* lightComponent =
      shovelerWorldEntityAddComponent(pointEntity, shovelerComponentTypeIdLight);
  shovelerComponentUpdateCanonicalFieldEntityId(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_POSITION, 5);
  shovelerComponentUpdateCanonicalFieldInt(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_TYPE, SHOVELER_COMPONENT_LIGHT_TYPE_POINT);
  shovelerComponentUpdateCanonicalFieldInt(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_WIDTH, 512);
  shovelerComponentUpdateCanonicalFieldInt(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_HEIGHT, 512);
  shovelerComponentUpdateCanonicalFieldInt(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_SAMPLES, 1);
  shovelerComponentUpdateCanonicalFieldFloat(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_AMBIENT_FACTOR, 0.0f);
  shovelerComponentUpdateCanonicalFieldFloat(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_EXPONENTIAL_FACTOR, 80.0f);
  shovelerComponentUpdateCanonicalFieldVector3(
      lightComponent, SHOVELER_COMPONENT_LIGHT_FIELD_ID_COLOR, shovelerVector3(1.0f, 1.0f, 1.0));
  shovelerComponentActivate(lightComponent);
  ShovelerComponent* lightPositionComponent =
      shovelerWorldEntityAddComponent(pointEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      lightPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      lightPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(0.0f, 2.0f, 0.0f));
  shovelerComponentActivate(lightPositionComponent);

  ShovelerWorldEntity* resourceEntity = shovelerWorldAddEntity(world, 6);
  ShovelerComponent* resourceComponent =
      shovelerWorldEntityAddComponent(resourceEntity, shovelerComponentTypeIdResource);
  ShovelerImage* image = shovelerImageCreate(2, 2, 4);
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
  GString* imageData = getImageData(image);
  shovelerComponentUpdateCanonicalFieldBytes(
      resourceComponent,
      SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER,
      (const unsigned char*) imageData->str,
      (int) imageData->len);
  g_string_free(imageData, true);
  shovelerComponentActivate(resourceComponent);
  ShovelerComponent* resourceImageComponent =
      shovelerWorldEntityAddComponent(resourceEntity, shovelerComponentTypeIdImage);
  shovelerComponentUpdateCanonicalFieldInt(
      resourceImageComponent,
      SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT,
      SHOVELER_COMPONENT_IMAGE_FORMAT_PNG);
  shovelerComponentUpdateCanonicalFieldEntityId(
      resourceImageComponent, SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE, 6);
  shovelerComponentActivate(resourceImageComponent);
  ShovelerComponent* resourceTextureComponent =
      shovelerWorldEntityAddComponent(resourceEntity, shovelerComponentTypeIdTexture);
  shovelerComponentUpdateCanonicalFieldInt(
      resourceTextureComponent,
      SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE);
  shovelerComponentUpdateCanonicalFieldEntityId(
      resourceTextureComponent, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE, 6);
  shovelerComponentActivate(resourceTextureComponent);
  ShovelerComponent* resourceSamplerComponent =
      shovelerWorldEntityAddComponent(resourceEntity, shovelerComponentTypeIdSampler);
  shovelerComponentUpdateCanonicalFieldBool(
      resourceSamplerComponent, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_INTERPOLATE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      resourceSamplerComponent, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_USE_MIPMAPS, true);
  shovelerComponentUpdateCanonicalFieldBool(
      resourceSamplerComponent, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_CLAMP, true);
  shovelerComponentActivate(resourceSamplerComponent);

  ShovelerWorldEntity* textureMaterialEntity = shovelerWorldAddEntity(world, 7);
  ShovelerComponent* textureMaterialComponent =
      shovelerWorldEntityAddComponent(textureMaterialEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      textureMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE);
  shovelerComponentUpdateCanonicalFieldInt(
      textureMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_TYPE,
      SHOVELER_MATERIAL_TEXTURE_TYPE_PHONG);
  shovelerComponentUpdateCanonicalFieldEntityId(
      textureMaterialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE, 6);
  shovelerComponentUpdateCanonicalFieldEntityId(
      textureMaterialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SAMPLER, 6);
  shovelerComponentActivate(textureMaterialComponent);

  ShovelerWorldEntity* layerEntity = shovelerWorldAddEntity(world, 8);
  ShovelerComponent* layerTilesComponent =
      shovelerWorldEntityAddComponent(layerEntity, shovelerComponentTypeIdTilemapTiles);
  shovelerComponentClearField(
      layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, /* isCaononical */ true);
  shovelerComponentUpdateCanonicalFieldInt(
      layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS, 2);
  shovelerComponentUpdateCanonicalFieldInt(
      layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS, 2);
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
  shovelerComponentUpdateCanonicalFieldBytes(
      layerTilesComponent,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS,
      tilesetColumns,
      4);
  shovelerComponentUpdateCanonicalFieldBytes(
      layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, tilesetRows, 4);
  shovelerComponentUpdateCanonicalFieldBytes(
      layerTilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, tilesetIds, 4);
  shovelerComponentActivate(layerTilesComponent);
  ShovelerComponent* layerCollidersComponent =
      shovelerWorldEntityAddComponent(layerEntity, shovelerComponentTypeIdTilemapColliders);
  shovelerComponentUpdateCanonicalFieldInt(
      layerCollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, 2);
  shovelerComponentUpdateCanonicalFieldInt(
      layerCollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, 2);
  unsigned char colliders[] = {false, false, false, false};
  shovelerComponentUpdateCanonicalFieldBytes(
      layerCollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, colliders, 4);
  shovelerComponentActivate(layerCollidersComponent);

  ShovelerWorldEntity* layer2ResourceEntity = shovelerWorldAddEntity(world, 9);
  ShovelerComponent* layer2ResourceComponent =
      shovelerWorldEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdResource);
  ShovelerImage* layer2Image = shovelerImageCreate(3, 3, 3);
  shovelerImageClear(layer2Image);
  shovelerImageGet(layer2Image, 0, 0, 0) = 0;
  shovelerImageGet(layer2Image, 0, 0, 1) = 0;
  shovelerImageGet(layer2Image, 0, 0, 2) = 2; // full tileset
  GString* layer2ImageData = getImageData(layer2Image);
  shovelerImageFree(layer2Image);
  shovelerComponentUpdateCanonicalFieldBytes(
      layer2ResourceComponent,
      SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER,
      (const unsigned char*) layer2ImageData->str,
      (int) layer2ImageData->len);
  g_string_free(layer2ImageData, true);
  shovelerComponentActivate(layer2ResourceComponent);
  ShovelerComponent* layer2ResourceImageComponent =
      shovelerWorldEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdImage);
  shovelerComponentUpdateCanonicalFieldInt(
      layer2ResourceImageComponent,
      SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT,
      SHOVELER_COMPONENT_IMAGE_FORMAT_PNG);
  shovelerComponentUpdateCanonicalFieldEntityId(
      layer2ResourceImageComponent, SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE, 9);
  shovelerComponentActivate(layer2ResourceImageComponent);
  ShovelerComponent* layer2TilesComponent =
      shovelerWorldEntityAddComponent(layer2ResourceEntity, shovelerComponentTypeIdTilemapTiles);
  shovelerComponentUpdateCanonicalFieldEntityId(
      layer2TilesComponent, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE, 9);
  shovelerComponentActivate(layer2TilesComponent);
  ShovelerComponent* layer2CollidersComponent = shovelerWorldEntityAddComponent(
      layer2ResourceEntity, shovelerComponentTypeIdTilemapColliders);
  shovelerComponentUpdateCanonicalFieldInt(
      layer2CollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, 3);
  shovelerComponentUpdateCanonicalFieldInt(
      layer2CollidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, 3);
  unsigned char colliders2[] = {false, false, false, false, false, false, false, false, false};
  shovelerComponentUpdateCanonicalFieldBytes(
      layer2CollidersComponent,
      SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS,
      colliders2,
      9);
  shovelerComponentActivate(layer2CollidersComponent);

  ShovelerWorldEntity* tilesetMaterialEntity = shovelerWorldAddEntity(world, 10);
  ShovelerComponent* tilesetComponent =
      shovelerWorldEntityAddComponent(tilesetMaterialEntity, shovelerComponentTypeIdTileset);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_IMAGE, 6);
  shovelerComponentUpdateCanonicalFieldInt(
      tilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_COLUMNS, 2);
  shovelerComponentUpdateCanonicalFieldInt(
      tilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_ROWS, 2);
  shovelerComponentUpdateCanonicalFieldInt(
      tilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_PADDING, 1);
  shovelerComponentActivate(tilesetComponent);

  ShovelerWorldEntity* animationTilesetEntity = shovelerWorldAddEntity(world, 11);
  ShovelerComponent* animationResourceComponent =
      shovelerWorldEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdResource);
  ShovelerImage* animationTilesetImage = shovelerImageCreateAnimationTileset(image, 1);
  shovelerImageFree(image);
  GString* animationTilesetImageData = getImageData(animationTilesetImage);
  shovelerImageFree(animationTilesetImage);
  shovelerComponentUpdateCanonicalFieldBytes(
      animationResourceComponent,
      SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER,
      (const unsigned char*) animationTilesetImageData->str,
      (int) animationTilesetImageData->len);
  g_string_free(animationTilesetImageData, true);
  shovelerComponentActivate(animationResourceComponent);
  ShovelerComponent* animationImageComponent =
      shovelerWorldEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdImage);
  shovelerComponentUpdateCanonicalFieldInt(
      animationImageComponent,
      SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT,
      SHOVELER_COMPONENT_IMAGE_FORMAT_PNG);
  shovelerComponentUpdateCanonicalFieldEntityId(
      animationImageComponent, SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE, 11);
  shovelerComponentActivate(animationImageComponent);
  ShovelerComponent* animationTextureComponent =
      shovelerWorldEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdTexture);
  shovelerComponentUpdateCanonicalFieldInt(
      animationTextureComponent,
      SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE);
  shovelerComponentUpdateCanonicalFieldEntityId(
      animationTextureComponent, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE, 11);
  shovelerComponentActivate(animationTextureComponent);
  ShovelerComponent* animationTilesetComponent =
      shovelerWorldEntityAddComponent(animationTilesetEntity, shovelerComponentTypeIdTileset);
  shovelerComponentUpdateCanonicalFieldEntityId(
      animationTilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_IMAGE, 11);
  shovelerComponentUpdateCanonicalFieldInt(
      animationTilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_COLUMNS, 4);
  shovelerComponentUpdateCanonicalFieldInt(
      animationTilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_ROWS, 3);
  shovelerComponentUpdateCanonicalFieldInt(
      animationTilesetComponent, SHOVELER_COMPONENT_TILESET_FIELD_ID_PADDING, 1);
  shovelerComponentActivate(animationTilesetComponent);

  ShovelerWorldEntity* tilemapBackgroundEntity = shovelerWorldAddEntity(world, 12);
  ShovelerComponent* tilemapBackgroundComponent =
      shovelerWorldEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdTilemap);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILES, 8);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_COLLIDERS, 8);
  long long int tilesetEntityIds[] = {10, 11};
  shovelerComponentUpdateCanonicalFieldEntityIdArray(
      tilemapBackgroundComponent,
      SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS,
      tilesetEntityIds,
      2);
  shovelerComponentActivate(tilemapBackgroundComponent);
  ShovelerComponent* tilemapSpriteBackgroundComponent = shovelerWorldEntityAddComponent(
      tilemapBackgroundEntity, shovelerComponentTypeIdTilemapSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapSpriteBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_MATERIAL, 12);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapSpriteBackgroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_TILEMAP, 12);
  shovelerComponentActivate(tilemapSpriteBackgroundComponent);
  ShovelerComponent* backgroundSpriteComponent =
      shovelerWorldEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, 12);
  shovelerComponentUpdateCanonicalFieldInt(
      backgroundSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
  shovelerComponentUpdateCanonicalFieldInt(
      backgroundSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
  shovelerComponentUpdateCanonicalFieldEntityId(
      backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, 16);
  shovelerComponentUpdateCanonicalFieldInt(
      backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, 0);
  shovelerComponentUpdateCanonicalFieldVector2(
      backgroundSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE,
      shovelerVector2(10.0f, 10.0f));
  shovelerComponentUpdateCanonicalFieldEntityId(
      backgroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE, 12);
  shovelerComponentActivate(backgroundSpriteComponent);
  ShovelerComponent* backgrouindTilemapMaterialComponent =
      shovelerWorldEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      backgrouindTilemapMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP);
  shovelerComponentActivate(backgrouindTilemapMaterialComponent);
  ShovelerComponent* backgroundSpritePositionComponent =
      shovelerWorldEntityAddComponent(tilemapBackgroundEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      backgroundSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      backgroundSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(5.0f, 5.0f, 0.0f));
  shovelerComponentActivate(backgroundSpritePositionComponent);

  ShovelerWorldEntity* tilemapForegroundEntity = shovelerWorldAddEntity(world, 13);
  ShovelerComponent* tilemapForegroundComponent =
      shovelerWorldEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdTilemap);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapForegroundComponent, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILES, 9);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapForegroundComponent, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_COLLIDERS, 9);
  shovelerComponentUpdateCanonicalFieldEntityIdArray(
      tilemapForegroundComponent,
      SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS,
      tilesetEntityIds,
      2);
  shovelerComponentActivate(tilemapForegroundComponent);
  ShovelerComponent* tilemapSpriteForegroundComponent = shovelerWorldEntityAddComponent(
      tilemapForegroundEntity, shovelerComponentTypeIdTilemapSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapSpriteForegroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_MATERIAL, 12);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapSpriteForegroundComponent, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_TILEMAP, 13);
  shovelerComponentActivate(tilemapSpriteForegroundComponent);
  ShovelerComponent* foregroundSpriteComponent =
      shovelerWorldEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, 13);
  shovelerComponentUpdateCanonicalFieldInt(
      foregroundSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
  shovelerComponentUpdateCanonicalFieldInt(
      foregroundSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
  shovelerComponentUpdateCanonicalFieldEntityId(
      foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, 16);
  shovelerComponentUpdateCanonicalFieldInt(
      foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, 2);
  shovelerComponentUpdateCanonicalFieldVector2(
      foregroundSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE,
      shovelerVector2(10.0f, 10.0f));
  shovelerComponentUpdateCanonicalFieldEntityId(
      foregroundSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE, 13);
  shovelerComponentActivate(foregroundSpriteComponent);
  ShovelerComponent* foregroundSpritePositionComponent =
      shovelerWorldEntityAddComponent(tilemapForegroundEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      foregroundSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      foregroundSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(5.0f, 5.0f, 0.0f));
  shovelerComponentActivate(foregroundSpritePositionComponent);

  ShovelerWorldEntity* tileSpriteEntity = shovelerWorldAddEntity(world, 14);
  ShovelerComponent* spriteComponent =
      shovelerWorldEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      spriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, 14);
  shovelerComponentUpdateCanonicalFieldInt(
      spriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
  shovelerComponentUpdateCanonicalFieldInt(
      spriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
  shovelerComponentUpdateCanonicalFieldEntityId(
      spriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, 15);
  shovelerComponentUpdateCanonicalFieldInt(
      spriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, 0);
  shovelerComponentUpdateCanonicalFieldVector2(
      spriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE, shovelerVector2(2.5f, 2.5f));
  shovelerComponentUpdateCanonicalFieldEntityId(
      spriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE, 14);
  shovelerComponentActivate(spriteComponent);
  ShovelerComponent* tileSpriteComponent =
      shovelerWorldEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdTileSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_MATERIAL, 14);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET, 11);
  shovelerComponentUpdateCanonicalFieldInt(
      tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_COLUMN, 0);
  shovelerComponentUpdateCanonicalFieldInt(
      tileSpriteComponent, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_ROW, 0);
  shovelerComponentActivate(tileSpriteComponent);
  ShovelerComponent* tileSpriteAnimationComponent =
      shovelerWorldEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdTileSpriteAnimation);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tileSpriteAnimationComponent, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION, 14);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tileSpriteAnimationComponent,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE,
      14);
  shovelerComponentUpdateCanonicalFieldInt(
      tileSpriteAnimationComponent,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
  shovelerComponentUpdateCanonicalFieldInt(
      tileSpriteAnimationComponent,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
  shovelerComponentUpdateCanonicalFieldFloat(
      tileSpriteAnimationComponent,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD,
      1.0f);
  shovelerComponentActivate(tileSpriteAnimationComponent);
  ShovelerComponent* tileSpriteMaterialComponent =
      shovelerWorldEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      tileSpriteMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TILE_SPRITE);
  shovelerComponentActivate(tileSpriteMaterialComponent);
  ShovelerComponent* tileSpritePositionComponent =
      shovelerWorldEntityAddComponent(tileSpriteEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      tileSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      tileSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(3.0f, 4.0f, 0.0f));
  shovelerComponentActivate(tileSpritePositionComponent);

  ShovelerWorldEntity* canvasEntity = shovelerWorldAddEntity(world, 15);
  ShovelerComponent* canvasComponent =
      shovelerWorldEntityAddComponent(canvasEntity, shovelerComponentTypeIdCanvas);
  shovelerComponentUpdateCanonicalFieldInt(
      canvasComponent, SHOVELER_COMPONENT_CANVAS_FIELD_ID_NUM_LAYERS, 1);
  shovelerComponentActivate(canvasComponent);

  ShovelerWorldEntity* chunkEntity = shovelerWorldAddEntity(world, 16);
  ShovelerComponent* chunkCanvasComponent =
      shovelerWorldEntityAddComponent(chunkEntity, shovelerComponentTypeIdCanvas);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkCanvasComponent, SHOVELER_COMPONENT_CANVAS_FIELD_ID_NUM_LAYERS, 3);
  shovelerComponentActivate(chunkCanvasComponent);

  ShovelerWorldEntity* chunkTileSpriteEntity = shovelerWorldAddEntity(world, 17);
  ShovelerComponent* chunkSpriteComponent =
      shovelerWorldEntityAddComponent(chunkTileSpriteEntity, shovelerComponentTypeIdSprite);
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, 14);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkSpriteComponent,
      SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y);
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, 16);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, 1);
  shovelerComponentUpdateCanonicalFieldVector2(
      chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE, shovelerVector2(2.5f, 2.5f));
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkSpriteComponent, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE, 14);
  shovelerComponentActivate(chunkSpriteComponent);

  ShovelerWorldEntity* tilemapMaterialEntity = shovelerWorldAddEntity(world, 20);
  ShovelerComponent* tilemapMaterialModelComponent =
      shovelerWorldEntityAddComponent(tilemapMaterialEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 20);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 4);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 20);
  shovelerComponentUpdateCanonicalFieldVector3(
      tilemapMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION,
      shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
  shovelerComponentUpdateCanonicalFieldVector3(
      tilemapMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE,
      shovelerVector3(0.5f, 0.5f, 1.0f));
  shovelerComponentUpdateCanonicalFieldBool(
      tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, false);
  shovelerComponentUpdateCanonicalFieldBool(
      tilemapMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, true);
  shovelerComponentUpdateCanonicalFieldInt(
      tilemapMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(tilemapMaterialModelComponent);
  ShovelerComponent* tilemapMaterialComponent =
      shovelerWorldEntityAddComponent(tilemapMaterialEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      tilemapMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP);
  shovelerComponentUpdateCanonicalFieldEntityId(
      tilemapMaterialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TILEMAP, 12);
  shovelerComponentActivate(tilemapMaterialComponent);
  ShovelerComponent* tilemapMaterialPositionComponent =
      shovelerWorldEntityAddComponent(tilemapMaterialEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      tilemapMaterialPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      tilemapMaterialPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(5.0f, 5.0f, 7.5f));
  shovelerComponentActivate(tilemapMaterialPositionComponent);

  ShovelerWorldEntity* canvasMaterialEntity = shovelerWorldAddEntity(world, 21);
  ShovelerComponent* canvasMaterialModelComponent =
      shovelerWorldEntityAddComponent(canvasMaterialEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 21);
  shovelerComponentUpdateCanonicalFieldEntityId(
      canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 4);
  shovelerComponentUpdateCanonicalFieldEntityId(
      canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 21);
  shovelerComponentUpdateCanonicalFieldVector3(
      canvasMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION,
      shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
  shovelerComponentUpdateCanonicalFieldVector3(
      canvasMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE,
      shovelerVector3(0.5f, 0.5f, 1.0f));
  shovelerComponentUpdateCanonicalFieldBool(
      canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, false);
  shovelerComponentUpdateCanonicalFieldBool(
      canvasMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, true);
  shovelerComponentUpdateCanonicalFieldInt(
      canvasMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(canvasMaterialModelComponent);
  ShovelerComponent* canvasMaterialComponent =
      shovelerWorldEntityAddComponent(canvasMaterialEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      canvasMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS);
  shovelerComponentUpdateCanonicalFieldEntityId(
      canvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS, 15);
  shovelerComponentUpdateCanonicalFieldVector2(
      canvasMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION,
      shovelerVector2(5.0f, 5.0f));
  shovelerComponentUpdateCanonicalFieldVector2(
      canvasMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE,
      shovelerVector2(10.0f, 10.0f));
  shovelerComponentActivate(canvasMaterialComponent);
  ShovelerComponent* canvasMaterialPositionComponent =
      shovelerWorldEntityAddComponent(canvasMaterialEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      canvasMaterialPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      canvasMaterialPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(0.0f, 5.0f, 7.5f));
  shovelerComponentActivate(canvasMaterialPositionComponent);

  ShovelerWorldEntity* chunkMaterialEntity = shovelerWorldAddEntity(world, 22);
  ShovelerComponent* chunkMaterialModelComponent =
      shovelerWorldEntityAddComponent(chunkMaterialEntity, shovelerComponentTypeIdModel);
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, 22);
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, 4);
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, 22);
  shovelerComponentUpdateCanonicalFieldVector3(
      chunkMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION,
      shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
  shovelerComponentUpdateCanonicalFieldVector3(
      chunkMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE,
      shovelerVector3(0.5f, 0.5f, 1.0f));
  shovelerComponentUpdateCanonicalFieldBool(
      chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, true);
  shovelerComponentUpdateCanonicalFieldBool(
      chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, false);
  shovelerComponentUpdateCanonicalFieldBool(
      chunkMaterialModelComponent, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, true);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkMaterialModelComponent,
      SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE,
      SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
  shovelerComponentActivate(chunkMaterialModelComponent);
  ShovelerComponent* chunkCanvasMaterialComponent =
      shovelerWorldEntityAddComponent(chunkMaterialEntity, shovelerComponentTypeIdMaterial);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkCanvasMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS);
  shovelerComponentUpdateCanonicalFieldEntityId(
      chunkCanvasMaterialComponent, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS, 16);
  shovelerComponentUpdateCanonicalFieldVector2(
      chunkCanvasMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION,
      shovelerVector2(5.0f, 5.0f));
  shovelerComponentUpdateCanonicalFieldVector2(
      chunkCanvasMaterialComponent,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE,
      shovelerVector2(10.0f, 10.0f));
  shovelerComponentActivate(chunkCanvasMaterialComponent);
  ShovelerComponent* chunkMaterialPositionComponent =
      shovelerWorldEntityAddComponent(chunkMaterialEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldInt(
      chunkMaterialPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      chunkMaterialPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(-5.0f, 5.0f, 7.5f));
  shovelerComponentActivate(chunkMaterialPositionComponent);

  ShovelerWorldEntity* clientEntity = shovelerWorldAddEntity(world, 23);
  ShovelerComponent* clientComponent =
      shovelerWorldEntityAddComponent(clientEntity, shovelerComponentTypeIdClient);
  shovelerComponentUpdateCanonicalFieldEntityId(
      clientComponent, SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION, 23);
  shovelerComponentDelegate(clientComponent);
  shovelerComponentActivate(clientComponent);
  ShovelerComponent* clientPositionComponent =
      shovelerWorldEntityAddComponent(clientEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldVector3(
      clientPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(0.0f, 0.0f, 0.0f));
  shovelerComponentDelegate(clientPositionComponent);
  shovelerComponentActivate(clientPositionComponent);

  shovelerOpenGLCheckSuccess();

  while (shovelerGameIsRunning(game)) {
    shovelerGameRenderFrame(game);
  }
  shovelerLogInfo("Exiting main loop, goodbye.");

  shovelerClientSystemFree(clientSystem);
  shovelerGameFree(game);
  shovelerGlobalUninit();
  shovelerLogTerminate();

  return EXIT_SUCCESS;
}

static GString* getImageData(ShovelerImage* image) {
  const char* tempImageFilename = "temp.png";
  shovelerImagePngWriteFile(image, tempImageFilename);

  unsigned char* contents;
  size_t contentsSize;
  shovelerFileRead(tempImageFilename, &contents, &contentsSize);

  GString* data = g_string_new("");
  g_string_append_len(data, (gchar*) contents, contentsSize);
  free(contents);

  return data;
}

static void updateGame(ShovelerGame* game, double dt) {
  shovelerCameraUpdateView(game->camera);

  time += dt;
  ShovelerWorldEntity* lightEntity = shovelerWorldGetEntity(world, 5);
  ShovelerComponent* lightPositionComponent =
      shovelerWorldEntityGetComponent(lightEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldVector3(
      lightPositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(2.0 * sin(time), 2.0 * cos(time), 0.0));

  ShovelerWorldEntity* tileSpriteEntity = shovelerWorldGetEntity(world, 14);
  ShovelerComponent* tileSpritePositionComponent =
      shovelerWorldEntityGetComponent(tileSpriteEntity, shovelerComponentTypeIdPosition);
  shovelerComponentUpdateCanonicalFieldVector3(
      tileSpritePositionComponent,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
      shovelerVector3(3.0 + 2.0 * sin(time), 4.0 + 2.0 * cos(time), 0.0));

  ShovelerWorldEntity* tilemapEntity = shovelerWorldGetEntity(world, 8);
  ShovelerComponent* tilemapComponent =
      shovelerWorldEntityGetComponent(tilemapEntity, shovelerComponentTypeIdTilemapTiles);
  const unsigned char* tilesetColumns;
  shovelerComponentGetFieldValueBytes(
      tilemapComponent,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS,
      &tilesetColumns,
      /* outputSize */ NULL);
  unsigned char updatedTilesetColumns[4];
  memcpy(&updatedTilesetColumns, tilesetColumns, 4 * sizeof(unsigned char));
  updatedTilesetColumns[0] = (unsigned char) ((int) time % 2);

  shovelerComponentUpdateCanonicalFieldBytes(
      tilemapComponent,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS,
      updatedTilesetColumns,
      4);
}

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
