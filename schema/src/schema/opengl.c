#include "shoveler/schema/opengl.h"

#include <assert.h>

#include "shoveler/component_field.h"
#include "shoveler/component_type.h"
#include "shoveler/schema.h"
#include "shoveler/schema/base.h"

const char* const shovelerComponentTypeIdCanvas = "canvas";
const char* const shovelerComponentTypeIdClient = "client";
const char* const shovelerComponentTypeIdDrawable = "drawable";
const char* const shovelerComponentTypeIdFontAtlasTexture = "font_atlas_texture";
const char* const shovelerComponentTypeIdLight = "light";
const char* const shovelerComponentTypeIdMaterial = "material";
const char* const shovelerComponentTypeIdModel = "model";
const char* const shovelerComponentTypeIdSampler = "sampler";
const char* const shovelerComponentTypeIdSprite = "sprite";
const char* const shovelerComponentTypeIdTextSprite = "text_sprite";
const char* const shovelerComponentTypeIdTextTextureRenderer = "text_texture_renderer";
const char* const shovelerComponentTypeIdTexture = "texture";
const char* const shovelerComponentTypeIdTextureSprite = "texture_sprite";
const char* const shovelerComponentTypeIdTileSprite = "tile_sprite";
const char* const shovelerComponentTypeIdTileSpriteAnimation = "tile_sprite_animation";
const char* const shovelerComponentTypeIdTilemap = "tilemap";
const char* const shovelerComponentTypeIdTilemapColliders = "tilemap_colliders";
const char* const shovelerComponentTypeIdTilemapSprite = "tilemap_sprite";
const char* const shovelerComponentTypeIdTilemapTiles = "tilemap_tiles";
const char* const shovelerComponentTypeIdTileset = "tileset";

static ShovelerComponentType* shovelerComponentCreateCanvasType();
static ShovelerComponentType* shovelerComponentCreateClientType();
static ShovelerComponentType* shovelerComponentCreateDrawableType();
static ShovelerComponentType* shovelerComponentCreateFontAtlasTextureType();
static ShovelerComponentType* shovelerComponentCreateLightType();
static ShovelerComponentType* shovelerComponentCreateMaterialType();
static ShovelerComponentType* shovelerComponentCreateModelType();
static ShovelerComponentType* shovelerComponentCreateSamplerType();
static ShovelerComponentType* shovelerComponentCreateSpriteType();
static ShovelerComponentType* shovelerComponentCreateTextSpriteType();
static ShovelerComponentType* shovelerComponentCreateTextTextureRendererType();
static ShovelerComponentType* shovelerComponentCreateTextureType();
static ShovelerComponentType* shovelerComponentCreateTextureSpriteType();
static ShovelerComponentType* shovelerComponentCreateTileSpriteType();
static ShovelerComponentType* shovelerComponentCreateTileSpriteAnimationType();
static ShovelerComponentType* shovelerComponentCreateTilemapType();
static ShovelerComponentType* shovelerComponentCreateTilemapCollidersType();
static ShovelerComponentType* shovelerComponentCreateTilemapSpriteType();
static ShovelerComponentType* shovelerComponentCreateTilemapTilesType();
static ShovelerComponentType* shovelerComponentCreateTilesetType();

void shovelerSchemaOpenglRegister(ShovelerSchema* schema) {
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateCanvasType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateClientType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateDrawableType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateFontAtlasTextureType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateLightType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateMaterialType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateModelType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateSamplerType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateSpriteType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTextSpriteType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTextTextureRendererType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTextureType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTextureSpriteType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTileSpriteType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTileSpriteAnimationType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTilemapType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTilemapCollidersType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTilemapSpriteType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTilemapTilesType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateTilesetType());
}

static ShovelerComponentType* shovelerComponentCreateCanvasType() {
  ShovelerComponentField fields[1];
  fields[SHOVELER_COMPONENT_CANVAS_FIELD_ID_NUM_LAYERS] = shovelerComponentField(
      "num_layers",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdCanvas, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateClientType() {
  ShovelerComponentField fields[2];
  fields[SHOVELER_COMPONENT_CLIENT_FIELD_ID_POSITION] = shovelerComponentFieldDependency(
      "position",
      shovelerComponentTypeIdPosition,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_CLIENT_FIELD_ID_MODEL] = shovelerComponentFieldDependency(
      "model",
      shovelerComponentTypeIdModel,
      /* isArray */ false,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdClient, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateDrawableType() {
  ShovelerComponentField fields[3];
  fields[SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TYPE] = shovelerComponentField(
      "type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TILES_WIDTH] = shovelerComponentField(
      "tiles_width",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_DRAWABLE_FIELD_ID_TILES_HEIGHT] = shovelerComponentField(
      "tiles_height",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdDrawable, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateFontAtlasTextureType() {
  ShovelerComponentField fields[1];
  fields[SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_FIELD_ID_FONT_ATLAS] =
      shovelerComponentFieldDependency(
          "font_atlas",
          shovelerComponentTypeIdFontAtlas,
          /* isArray */ false,
          /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdFontAtlasTexture, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateLightType() {
  ShovelerComponentField fields[8];
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_POSITION] = shovelerComponentFieldDependency(
      "position",
      shovelerComponentTypeIdPosition,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_TYPE] = shovelerComponentField(
      "type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_WIDTH] = shovelerComponentField(
      "width",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_HEIGHT] = shovelerComponentField(
      "height",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_SAMPLES] = shovelerComponentField(
      "samples",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_AMBIENT_FACTOR] = shovelerComponentField(
      "ambient_factor",
      SHOVELER_COMPONENT_FIELD_TYPE_FLOAT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_EXPONENTIAL_FACTOR] = shovelerComponentField(
      "exponential_factor",
      SHOVELER_COMPONENT_FIELD_TYPE_FLOAT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_LIGHT_FIELD_ID_COLOR] = shovelerComponentField(
      "tiles_height",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdLight, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateMaterialType() {
  ShovelerComponentField fields[10];
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE] = shovelerComponentField(
      "type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_TYPE] = shovelerComponentField(
      "texture_type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SPRITE_TYPE] = shovelerComponentField(
      "texture_sprite_type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE] = shovelerComponentFieldDependency(
      "texture",
      shovelerComponentTypeIdTexture,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SAMPLER] = shovelerComponentFieldDependency(
      "texture_sampler",
      shovelerComponentTypeIdSampler,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TILEMAP] = shovelerComponentFieldDependency(
      "tilemap",
      shovelerComponentTypeIdTilemap,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS] = shovelerComponentFieldDependency(
      "canvas",
      shovelerComponentTypeIdCanvas,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR] = shovelerComponentField(
      "color",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION] = shovelerComponentField(
      "canvas_region_position",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE] = shovelerComponentField(
      "canvas_region_size",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdMaterial, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateModelType() {
  ShovelerComponentField fields[9];
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION] = shovelerComponentFieldDependency(
      "position",
      shovelerComponentTypeIdPosition,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE] = shovelerComponentFieldDependency(
      "drawable",
      shovelerComponentTypeIdDrawable,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL] = shovelerComponentFieldDependency(
      "material",
      shovelerComponentTypeIdMaterial,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION] = shovelerComponentField(
      "rotation",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE] = shovelerComponentField(
      "scale",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE] = shovelerComponentField(
      "visible",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER] = shovelerComponentField(
      "emitter",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW] = shovelerComponentField(
      "casts_shadow",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE] = shovelerComponentField(
      "polygon_mode",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdModel, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateSamplerType() {
  ShovelerComponentField fields[3];
  fields[SHOVELER_COMPONENT_SAMPLER_FIELD_ID_INTERPOLATE] = shovelerComponentField(
      "interpolate",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_SAMPLER_FIELD_ID_USE_MIPMAPS] = shovelerComponentField(
      "use_mipmaps",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_SAMPLER_FIELD_ID_CLAMP] = shovelerComponentField(
      "clamp",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdSampler, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateSpriteType() {
  ShovelerComponentField fields[11];
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION] = shovelerComponentFieldDependency(
      "position",
      shovelerComponentTypeIdPosition,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_X] = shovelerComponentField(
      "position_mapping_x",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_POSITION_MAPPING_Y] = shovelerComponentField(
      "position_mapping_y",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_ENABLE_COLLIDER] = shovelerComponentField(
      "enable_collider",
      SHOVELER_COMPONENT_FIELD_TYPE_BOOL,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_SIZE] = shovelerComponentField(
      "size",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_CANVAS] = shovelerComponentFieldDependency(
      "canvas",
      shovelerComponentTypeIdCanvas,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_LAYER] = shovelerComponentField(
      "layer",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_TEXT_SPRITE] = shovelerComponentFieldDependency(
      "text_sprite",
      shovelerComponentTypeIdTextSprite,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILE_SPRITE] = shovelerComponentFieldDependency(
      "tile_sprite",
      shovelerComponentTypeIdTileSprite,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_TILEMAP_SPRITE] = shovelerComponentFieldDependency(
      "tilemap_sprite",
      shovelerComponentTypeIdTilemapSprite,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_SPRITE_FIELD_ID_TEXTURE_SPRITE] = shovelerComponentFieldDependency(
      "texture_sprite",
      shovelerComponentTypeIdTextureSprite,
      /* isArray */ false,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdSprite, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTextSpriteType() {
  ShovelerComponentField fields[5];
  fields[SHOVELER_COMPONENT_TEXT_SPRITE_FIELD_ID_MATERIAL] = shovelerComponentFieldDependency(
      "material",
      shovelerComponentTypeIdMaterial,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXT_SPRITE_FIELD_ID_FONT_ATLAS] = shovelerComponentFieldDependency(
      "font_atlas",
      /* TODO */ shovelerComponentTypeIdPosition,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXT_SPRITE_FIELD_ID_FONT_SIZE] = shovelerComponentField(
      "font_size",
      SHOVELER_COMPONENT_FIELD_TYPE_FLOAT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXT_SPRITE_FIELD_ID_CONTENT] = shovelerComponentField(
      "content",
      SHOVELER_COMPONENT_FIELD_TYPE_STRING,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXT_SPRITE_FIELD_ID_COLOR] = shovelerComponentField(
      "color",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR4,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTextSprite, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTextTextureRendererType() {
  ShovelerComponentField fields[1];
  fields[SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_FIELD_ID_FONT_ATLAS_TEXTURE] =
      shovelerComponentFieldDependency(
          "font_atlas",
          shovelerComponentTypeIdFontAtlasTexture,
          /* isArray */ false,
          /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTextTextureRenderer, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTextureType() {
  ShovelerComponentField fields[4];
  fields[SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE] = shovelerComponentField(
      "type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE] = shovelerComponentFieldDependency(
      "image",
      shovelerComponentTypeIdImage,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT_TEXTURE_RENDERER] =
      shovelerComponentFieldDependency(
          "text_texture_renderer",
          shovelerComponentTypeIdTextTextureRenderer,
          /* isArray */ false,
          /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT] = shovelerComponentField(
      "text",
      SHOVELER_COMPONENT_FIELD_TYPE_STRING,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTexture, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTextureSpriteType() {
  ShovelerComponentField fields[4];
  fields[SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_MATERIAL] = shovelerComponentFieldDependency(
      "material",
      shovelerComponentTypeIdMaterial,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_TEXTURE] = shovelerComponentFieldDependency(
      "texture",
      shovelerComponentTypeIdTexture,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_SAMPLER] = shovelerComponentFieldDependency(
      "sampler",
      shovelerComponentTypeIdSampler,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_SCALE] = shovelerComponentField(
      "scale",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR2,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTextureSprite, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTileSpriteType() {
  ShovelerComponentField fields[4];
  fields[SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_MATERIAL] = shovelerComponentFieldDependency(
      "material",
      shovelerComponentTypeIdMaterial,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET] = shovelerComponentFieldDependency(
      "tileset",
      shovelerComponentTypeIdTileset,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_COLUMN] = shovelerComponentField(
      "tileset_column",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_FIELD_ID_TILESET_ROW] = shovelerComponentField(
      "tileset_row",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTileSprite, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTileSpriteAnimationType() {
  ShovelerComponentField fields[5];
  fields[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION] =
      shovelerComponentFieldDependency(
          "position",
          shovelerComponentTypeIdPosition,
          /* isArray */ false,
          /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE] =
      shovelerComponentFieldDependency(
          "tile_sprite",
          shovelerComponentTypeIdTileSprite,
          /* isArray */ false,
          /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X] =
      shovelerComponentField(
          "position_mapping_x",
          SHOVELER_COMPONENT_FIELD_TYPE_INT,
          /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y] =
      shovelerComponentField(
          "position_mapping_y",
          SHOVELER_COMPONENT_FIELD_TYPE_INT,
          /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD] =
      shovelerComponentField(
          "move_amount_threshold",
          SHOVELER_COMPONENT_FIELD_TYPE_FLOAT,
          /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTileSpriteAnimation, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTilemapType() {
  ShovelerComponentField fields[3];
  fields[SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILES] = shovelerComponentFieldDependency(
      "tiles",
      shovelerComponentTypeIdTilemapTiles,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILEMAP_FIELD_ID_COLLIDERS] = shovelerComponentFieldDependency(
      "colliders",
      shovelerComponentTypeIdTilemapColliders,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILEMAP_FIELD_ID_TILESETS] = shovelerComponentFieldDependency(
      "tilesets",
      shovelerComponentTypeIdTileset,
      /* isArray */ true,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTilemap, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTilemapCollidersType() {
  ShovelerComponentField fields[3];
  fields[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS] = shovelerComponentField(
      "num_columns",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS] = shovelerComponentField(
      "num_rows",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS] = shovelerComponentField(
      "colliders",
      SHOVELER_COMPONENT_FIELD_TYPE_BYTES,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTilemapColliders, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTilemapSpriteType() {
  ShovelerComponentField fields[2];
  fields[SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_MATERIAL] = shovelerComponentFieldDependency(
      "material",
      shovelerComponentTypeIdMaterial,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILEMAP_SPRITE_FIELD_ID_TILEMAP] = shovelerComponentFieldDependency(
      "tilemap",
      shovelerComponentTypeIdTilemap,
      /* isArray */ false,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTilemapSprite, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTilemapTilesType() {
  ShovelerComponentField fields[6];
  fields[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE] = shovelerComponentFieldDependency(
      "image",
      shovelerComponentTypeIdImage,
      /* isArray */ false,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS] = shovelerComponentField(
      "num_columns",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS] = shovelerComponentField(
      "num_rows",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS] = shovelerComponentField(
      "tileset_columns",
      SHOVELER_COMPONENT_FIELD_TYPE_BYTES,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS] = shovelerComponentField(
      "tileset_rows",
      SHOVELER_COMPONENT_FIELD_TYPE_BYTES,
      /* isOptional */ true);
  fields[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS] = shovelerComponentField(
      "tileset_ids",
      SHOVELER_COMPONENT_FIELD_TYPE_BYTES,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTilemapTiles, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateTilesetType() {
  ShovelerComponentField fields[4];
  fields[SHOVELER_COMPONENT_TILESET_FIELD_ID_IMAGE] = shovelerComponentFieldDependency(
      "image",
      shovelerComponentTypeIdImage,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_COLUMNS] = shovelerComponentField(
      "num_columns",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILESET_FIELD_ID_NUM_ROWS] = shovelerComponentField(
      "num_rows",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_TILESET_FIELD_ID_PADDING] = shovelerComponentField(
      "padding",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdTileset, sizeof(fields) / sizeof(fields[0]), fields);
}
