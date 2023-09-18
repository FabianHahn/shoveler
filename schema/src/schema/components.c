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
    ShovelerWorldEntity* entity, unsigned char* buffer, int bufferSize) {
  assert(bufferSize > 0);

  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdResource, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldBytes(
      component, SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER, buffer, bufferSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddImageComponent(
    ShovelerWorldEntity* entity, ShovelerComponentImageFormat format, long long int resource) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdImage, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT, format);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE, resource);
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

void shovelerWorldEntityAddTextureImageComponent(ShovelerWorldEntity* entity, long long int image) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTexture, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE, SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE, image);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilesetComponent(
    ShovelerWorldEntity* entity, long long int image, int numColumns, int numRows, int padding) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTileset, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_IMAGE, image);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_COLUMNS, numColumns);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_ROWS, numRows);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILESET_FIELD_ID_PADDING, padding);
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

void shovelerWorldEntityAddMaterialCanvasComponent(
    ShovelerWorldEntity* entity,
    long long int canvas,
    ShovelerVector2 regionPosition,
    ShovelerVector2 regionSize) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdMaterial, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component,
      SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE,
      SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS, canvas);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION, regionPosition);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE, regionSize);
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
    unsigned char* colliders,
    int collidersSize) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTilemapColliders, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS, numColumns);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS, numRows);
  shovelerComponentUpdateCanonicalFieldBytes(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS, colliders, collidersSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilemapTilesDirectComponent(
    ShovelerWorldEntity* entity,
    int numColumns,
    int numRows,
    unsigned char* columns,
    unsigned char* rows,
    unsigned char* ids) {
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
    long long int tiles,
    long long int colliders,
    long long int* tilesets,
    int tilesetsSize) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTilemap, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILES, tiles);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_COLLIDERS, colliders);
  shovelerComponentUpdateCanonicalFieldEntityIdArray(
      component, SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS, tilesets, tilesetsSize);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTilemapSpriteComponent(
    ShovelerWorldEntity* entity, long long int material, long long int tilemap) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTilemapSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_MATERIAL, material);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_TILEMAP, tilemap);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTileSpriteComponent(
    ShovelerWorldEntity* entity,
    long long int material,
    long long int tileset,
    int tilesetColumn,
    int tilesetRow) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdTileSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_MATERIAL, material);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET, tileset);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_COLUMN, tilesetColumn);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_ROW, tilesetRow);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddTileSpriteAnimationComponent(
    ShovelerWorldEntity* entity,
    long long int position,
    long long int tileSprite,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    float moveAmountThreshold) {
  ShovelerComponent* component = shovelerWorldEntityAddComponent(
      entity, shovelerComponentTypeIdTileSpriteAnimation, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION, position);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE, tileSprite);
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
    long long int position,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvas,
    int layer,
    ShovelerVector2 size,
    long long int tilemapSprite) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, position);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X, positionMappingX);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y, positionMappingY);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_ENABLE_COLLIDER, enableCollider);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, canvas);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, layer);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE, size);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE, tilemapSprite);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddSpriteTileComponent(
    ShovelerWorldEntity* entity,
    long long int position,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvas,
    int layer,
    ShovelerVector2 size,
    long long int tileSprite) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdSprite, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION, position);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X, positionMappingX);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y, positionMappingY);
  shovelerComponentUpdateCanonicalFieldBool(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_ENABLE_COLLIDER, enableCollider);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS, canvas);
  shovelerComponentUpdateCanonicalFieldInt(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER, layer);
  shovelerComponentUpdateCanonicalFieldVector2(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE, size);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE, tileSprite);
  shovelerComponentActivate(component);
}

void shovelerWorldEntityAddModelComponent(
    ShovelerWorldEntity* entity,
    long long int position,
    long long int drawable,
    long long int material,
    ShovelerVector3 rotation,
    ShovelerVector3 scale,
    bool visible,
    bool emitter,
    bool castsShadow,
    ShovelerComponentModelPolygonMode polygonMode) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdModel, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION, position);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE, drawable);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL, material);
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

void shovelerWorldEntityAddClientComponent(ShovelerWorldEntity* entity, long long int position) {
  ShovelerComponent* component =
      shovelerWorldEntityAddComponent(entity, shovelerComponentTypeIdClient, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component, SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION, position);
  shovelerComponentActivate(component);
}
