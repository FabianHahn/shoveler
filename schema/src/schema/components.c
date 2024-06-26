#include "shoveler/schema/components.h"

#include "shoveler/component.h"
#include "shoveler/component_field.h"
#include "shoveler/schema/base.h"
#include "shoveler/schema/opengl.h"

void shovelerWorldEntityAddPositionAbsoluteComponent(
    ShovelerWorldEntity* entity, ShovelerVector3 positionCoordinates) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdPosition, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
  shovelerComponentUpdateCanonicalFieldVector3(
      component, SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES, positionCoordinates);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddDrawableComponent(
    ShovelerWorldEntity* entity, ShovelerComponentDrawableType drawableType) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdDrawable, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE, drawableType);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddResourceComponent(
    ShovelerWorldEntity* entity, const unsigned char* buffer, int bufferSize) {
  assert(bufferSize > 0);

  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdResource, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldBytes(
      component, SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER, buffer, bufferSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddImageComponent(
    ShovelerWorldEntity* entity,
    ShovelerComponentImageFormat format,
    long long int resourceEntityId) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdImage, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT, format);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE, resourceEntityId);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddSamplerComponent(
    ShovelerWorldEntity* entity, bool interpolate, bool useMipmaps, bool clamp) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdSampler, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_INTERPOLATE, interpolate);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_USE_MIPMAPS, useMipmaps);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_CLAMP, clamp);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTextureImageComponent(
    ShovelerWorldEntity* entity, long long int imageEntityId) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTexture, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE, SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE, imageEntityId);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilesetComponent(
    ShovelerWorldEntity* entity,
    long long int imageEntityId,
    int numColumns,
    int numRows,
    int padding) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTileset, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_IMAGE, imageEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_COLUMNS, numColumns);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_ROWS, numRows);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_PADDING, padding);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddMaterialCanvasComponent(
    ShovelerWorldEntity* entity,
    long long int canvasEntityId,
    ShovelerVector2 position,
    ShovelerVector2 regionSize) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdMaterial, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS, canvasEntityId);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION, position);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE, regionSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddMaterialTileSpriteComponent(ShovelerWorldEntity* entity) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdMaterial, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TILE_SPRITE);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddMaterialTilemapComponent(ShovelerWorldEntity* entity) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdMaterial, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddCanvasComponent(ShovelerWorldEntity* entity, int numLayers) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdCanvas, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_CANVAS_FIELD_ID_NUM_LAYERS, numLayers);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilemapCollidersComponent(
    ShovelerWorldEntity* entity,
    int numColumns,
    int numRows,
    const unsigned char* collidersEntityIds,
    int collidersSize) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTilemapColliders, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, numColumns);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, numRows);
  shovelerComponentUpdateCanonicalFieldBytes(
      component,
      SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS,
      collidersEntityIds,
      collidersSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilemapTilesDirectComponent(
    ShovelerWorldEntity* entity,
    int numColumns,
    int numRows,
    const unsigned char* columns,
    const unsigned char* rows,
    const unsigned char* ids) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTilemapTiles, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS, numColumns);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS, numRows);
  shovelerComponentUpdateCanonicalFieldBytes(
      component,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS,
      columns,
      numColumns * numRows);
  shovelerComponentUpdateCanonicalFieldBytes(
      component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS, rows, numColumns * numRows);
  shovelerComponentUpdateCanonicalFieldBytes(
      component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS, ids, numColumns * numRows);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilemapComponent(
    ShovelerWorldEntity* entity,
    long long int tilesEntityId,
    long long int collidersEntityId,
    const long long int* tilesetsEntityIds,
    int tilesetsSize) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTilemap, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILES, tilesEntityId);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_COLLIDERS, collidersEntityId);
  shovelerComponentUpdateCanonicalFieldEntityIdArray(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS, tilesetsEntityIds, tilesetsSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilemapSpriteComponent(
    ShovelerWorldEntity* entity, long long int materialEntityId, long long int tilemapEntityId) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTilemapSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_MATERIAL, materialEntityId);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_TILEMAP, tilemapEntityId);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTileSpriteComponent(
    ShovelerWorldEntity* entity,
    long long int materialEntityId,
    long long int tilesetEntityId,
    int tilesetColumn,
    int tilesetRow) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTileSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_MATERIAL, materialEntityId);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET, tilesetEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_COLUMN, tilesetColumn);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_ROW, tilesetRow);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTileSpriteAnimationComponent(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    long long int tileSpriteEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    float moveAmountThreshold) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTileSpriteAnimation, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION, positionEntityId);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE, tileSpriteEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X,
      positionMappingX);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y,
      positionMappingY);
  shovelerComponentUpdateCanonicalFieldFloat(
      component,
      SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD,
      moveAmountThreshold);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddSpriteTilemapComponent(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvasEntityId,
    int layer,
    ShovelerVector2 size,
    long long int tilemapSpriteEntityId) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, positionEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X, positionMappingX);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y, positionMappingY);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_ENABLE_COLLIDER, enableCollider);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, canvasEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, layer);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE, size);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE, tilemapSpriteEntityId);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddSpriteTileComponent(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvasEntityId,
    int layer,
    ShovelerVector2 size,
    long long int tileSpriteEntityId) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, positionEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X, positionMappingX);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y, positionMappingY);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_ENABLE_COLLIDER, enableCollider);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, canvasEntityId);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, layer);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE, size);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE, tileSpriteEntityId);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddModelComponent(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    long long int drawableEntityId,
    long long int materialEntityId,
    ShovelerVector3 rotation,
    ShovelerVector3 scale,
    bool visible,
    bool emitter,
    bool castsShadow,
    ShovelerComponentModelPolygonMode polygonMode) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdModel, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, positionEntityId);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, drawableEntityId);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, materialEntityId);
  shovelerComponentUpdateCanonicalFieldVector3(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION, rotation);
  shovelerComponentUpdateCanonicalFieldVector3(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE, scale);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE, visible);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER, emitter);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW, castsShadow);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE, polygonMode);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddClientComponent(
    ShovelerWorldEntity* entity, long long int positionEntityId) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdClient, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION, positionEntityId);
  shovelerComponentActivate(component);
}
