#include "shoveler/tiles/seeder.h"

#include <stdlib.h>
#include <string.h>

#include "shoveler/entity_id_allocator.h"
#include "shoveler/file.h"
#include "shoveler/image/png.h"
#include "shoveler/map.h"
#include "shoveler/schema/components.h"
#include "shoveler/tiles/tileset.h"

static GString* getImageData(ShovelerImage* image);
static long long int addCharacterAnimationTilesetEntity(
    ShovelerWorld* world,
    ShovelerEntityIdAllocator* entityIdAllocator,
    const char* filename,
    int shiftAmount);

ShovelerTilesSeeder shovelerTilesSeederInit(
    ShovelerWorld* world,
    ShovelerMap* map,
    ShovelerEntityIdAllocator* entityIdAllocator,
    const char* tilesetPngFilename,
    int tilesetPngColumns,
    int tilesetPngRows,
    const char* character1PngFilename,
    const char* character2PngFilename,
    const char* character3PngFilename,
    const char* character4PngFilename,
    int characterShiftAmount) {
  ShovelerTilesSeeder seeder;
  seeder.world = world;
  seeder.map = map;
  seeder.entityIdAllocator = entityIdAllocator;

  seeder.quadDrawableEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
  { // quad drawable
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seeder.quadDrawableEntityId);
    entity->label = strdup("drawable");
    shovelerWorldEntityAddDrawableComponent(entity, SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD);
  }

  seeder.tilesetEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
  { // tileset
    ShovelerImage* tilesetImage;
    int tilesetColumns;
    int tilesetRows;
    shovelerTilesCreateTileset(&tilesetImage, &tilesetColumns, &tilesetRows);
    GString* imageData = getImageData(tilesetImage);
    shovelerImageFree(tilesetImage);

    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seeder.tilesetEntityId);
    entity->label = strdup("tileset");
    shovelerWorldEntityAddResourceComponent(
        entity, (const unsigned char*) imageData->str, (int) imageData->len);
    shovelerWorldEntityAddImageComponent(
        entity, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG, /* resource */ 0);
    shovelerWorldEntityAddSamplerComponent(
        entity, /* interpolate */ true, /* useMipmaps */ false, /* clamp */ true);
    shovelerWorldEntityAddTextureImageComponent(entity, /* image */ 0);
    shovelerWorldEntityAddTilesetComponent(
        entity, /* image */ 0, tilesetColumns, tilesetRows, /* padding */ 1);

    g_string_free(imageData, true);
  }

  seeder.tilesetPngEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
  { // tileset png
    ShovelerImage* tilesetPngImage = shovelerImagePngReadFile(tilesetPngFilename);
    GString* tilesetPngData = getImageData(tilesetPngImage);
    shovelerImageFree(tilesetPngImage);

    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seeder.tilesetPngEntityId);
    entity->label = strdup("tileset");
    shovelerWorldEntityAddResourceComponent(
        entity, (const unsigned char*) tilesetPngData->str, (int) tilesetPngData->len);
    shovelerWorldEntityAddImageComponent(
        entity, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG, /* resourceEntityId */ 0);
    shovelerWorldEntityAddSamplerComponent(
        entity,
        /* interpolate */ true,
        /* useMipmaps */ false,
        /* clamp */ true);
    shovelerWorldEntityAddTextureImageComponent(entity, /* image */ 0);
    shovelerWorldEntityAddTilesetComponent(
        entity,
        /* image */ 0,
        tilesetPngColumns,
        tilesetPngRows,
        /* padding */ 1);

    g_string_free(tilesetPngData, true);
  }

  seeder.characterTilesetEntityIds[0] = addCharacterAnimationTilesetEntity(
      world, entityIdAllocator, character1PngFilename, characterShiftAmount);
  seeder.characterTilesetEntityIds[1] = addCharacterAnimationTilesetEntity(
      world, entityIdAllocator, character2PngFilename, characterShiftAmount);
  seeder.characterTilesetEntityIds[2] = addCharacterAnimationTilesetEntity(
      world, entityIdAllocator, character3PngFilename, characterShiftAmount);
  seeder.characterTilesetEntityIds[3] = addCharacterAnimationTilesetEntity(
      world, entityIdAllocator, character4PngFilename, characterShiftAmount);

  seeder.canvasEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
  {
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seeder.canvasEntityId);
    entity->label = strdup("canvas");
    shovelerWorldEntityAddMaterialTileSpriteComponent(entity);
    shovelerWorldEntityAddCanvasComponent(entity, /* numLayers */ 3);
  }

  { // tile sprite material
    ShovelerWorldEntity* entity =
        shovelerWorldAddEntity(world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
    entity->label = strdup("material");
    shovelerWorldEntityAddMaterialTileSpriteComponent(entity);
  }

  seeder.tilemapMaterialEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
  { // tilemap material
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seeder.tilemapMaterialEntityId);
    entity->label = strdup("material");
    shovelerWorldEntityAddMaterialTilemapComponent(entity);
  }

  ShovelerVector2 chunkSize2d =
      shovelerVector2((float) map->dimensions.chunkSize, (float) map->dimensions.chunkSize);
  ShovelerVector3 chunkScale3d = shovelerVector3(
      0.5f * (float) map->dimensions.chunkSize, 0.5f * (float) map->dimensions.chunkSize, 1.0f);
  for (int chunkX = 0; chunkX < map->dimensions.numChunkColumns; chunkX++) {
    for (int chunkY = 0; chunkY < map->dimensions.numChunkRows; chunkY++) {
      ShovelerMapChunk* chunk = shovelerMapGetChunk(map, chunkX, chunkY);

      long long int tilesets[] = {seeder.tilesetEntityId, seeder.tilesetPngEntityId};
      { // chunk background
        ShovelerWorldEntity* entity =
            shovelerWorldAddEntity(world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
        entity->label = strdup("chunk background");
        shovelerWorldEntityAddPositionAbsoluteComponent(
            entity, shovelerVector3(chunk->position.values[0], chunk->position.values[1], 0.0f));
        shovelerWorldEntityAddTilemapCollidersComponent(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->backgroundTiles.tilesetColliders,
            /* collidersSize */ map->dimensions.chunkSize * map->dimensions.chunkSize);
        shovelerWorldEntityAddTilemapTilesDirectComponent(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->backgroundTiles.tilesetColumns,
            chunk->backgroundTiles.tilesetRows,
            chunk->backgroundTiles.tilesetIds);
        shovelerWorldEntityAddTilemapComponent(
            entity,
            /* tilesEntityId */ 0,
            /* collidersEntityId */ 0,
            tilesets,
            /* numTilesets */ 2);
        shovelerWorldEntityAddTilemapSpriteComponent(
            entity, seeder.tilemapMaterialEntityId, /* tilemapEntityId */ 0);
        shovelerWorldEntityAddSpriteTilemapComponent(
            entity,
            /* position */ 0,
            SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
            SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
            /* enableCollider */ true,
            seeder.canvasEntityId,
            /* layer */ 0,
            /* size */ chunkSize2d,
            /* tilemapSprite */ 0);
      }

      { // chunk foreground
        ShovelerWorldEntity* entity =
            shovelerWorldAddEntity(world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
        entity->label = strdup("chunk foreground");
        shovelerWorldEntityAddPositionAbsoluteComponent(
            entity, shovelerVector3(chunk->position.values[0], chunk->position.values[1], 0.0f));
        shovelerWorldEntityAddTilemapCollidersComponent(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->foregroundTiles.tilesetColliders,
            /* collidersSize */ map->dimensions.chunkSize * map->dimensions.chunkSize);
        shovelerWorldEntityAddTilemapTilesDirectComponent(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->foregroundTiles.tilesetColumns,
            chunk->foregroundTiles.tilesetRows,
            chunk->foregroundTiles.tilesetIds);
        shovelerWorldEntityAddTilemapComponent(
            entity,
            /* tilesEntityId */ 0,
            /* collidersEntityId */ 0,
            tilesets,
            /* numTilesets */ 2);
        shovelerWorldEntityAddTilemapSpriteComponent(
            entity, seeder.tilemapMaterialEntityId, /* tilemapEntityId */ 0);
        shovelerWorldEntityAddSpriteTilemapComponent(
            entity,
            /* position */ 0,
            SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
            SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
            /* enableCollider */ true,
            seeder.canvasEntityId,
            /* layer */ 0,
            /* size */ chunkSize2d,
            /* tilemapSprite */ 0);
      }

      { // chunk
        ShovelerWorldEntity* entity =
            shovelerWorldAddEntity(world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
        entity->label = strdup("chunk");
        shovelerWorldEntityAddPositionAbsoluteComponent(
            entity, shovelerVector3(chunk->position.values[0], chunk->position.values[1], 0.0f));
        shovelerWorldEntityAddMaterialCanvasComponent(
            entity, seeder.canvasEntityId, chunk->position, /* regionSize */ chunkSize2d);
        shovelerWorldEntityAddModelComponent(
            entity,
            /* position */ 0,
            seeder.quadDrawableEntityId,
            /* material */ 0,
            /* rotation */ shovelerVector3(0.0f, 0.0f, 0.0f),
            /* scale */ chunkScale3d,
            /* visible */ true,
            /* emitter */ true,
            /* castsShadow */ false,
            SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
      }
    }
  }

  seeder.nextPlayerTilesetIndex = 0;

  return seeder;
}

void shovelerTilesSeederSpawnPlayer(ShovelerTilesSeeder* seeder, ShovelerVector2 position) {
  long long int playerEntityId = shovelerEntityIdAllocatorAllocate(seeder->entityIdAllocator);

  ShovelerWorldEntity* entity = shovelerWorldAddEntity(seeder->world, playerEntityId);
  entity->label = strdup("player");
  shovelerWorldEntityAddPositionAbsoluteComponent(
      entity, shovelerVector3(position.values[0], position.values[1], 5.0f));
  shovelerWorldEntityDelegateComponent(entity, shovelerComponentTypeIdPosition);
  shovelerWorldEntityAddClientComponent(entity, /* position */ 0);
  shovelerWorldEntityAddTileSpriteComponent(
      entity,
      /* materialEntityId */ seeder->canvasEntityId,
      seeder->characterTilesetEntityIds[seeder->nextPlayerTilesetIndex],
      /* tilesetColumn */ 0,
      /* tilesetRow */ 0);
  shovelerWorldEntityAddTileSpriteAnimationComponent(
      entity,
      /* position */ 0,
      /* tileSprite */ 0,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
      /* moveAmountThreshold */ 0.5f);
  shovelerWorldEntityAddSpriteTileComponent(
      entity,
      /* position */ 0,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
      /* enableCollider */ false,
      seeder->canvasEntityId,
      /* layer */ 1,
      shovelerVector2(1.0f, 1.0f),
      /* tileSprite */ 0);

  seeder->nextPlayerTilesetIndex = (seeder->nextPlayerTilesetIndex + 1) % 4;
}

static GString* getImageData(ShovelerImage* image) {
  const char* tempImageFilename = "temp.png";
  shovelerImagePngWriteFile(image, tempImageFilename);

  unsigned char* contents;
  size_t contentsSize;
  shovelerFileRead(tempImageFilename, &contents, &contentsSize);

  GString* data = g_string_new("");
  g_string_append_len(data, (gchar*) contents, (gssize) contentsSize);
  free(contents);

  return data;
}

static long long int addCharacterAnimationTilesetEntity(
    ShovelerWorld* world,
    ShovelerEntityIdAllocator* entityIdAllocator,
    const char* filename,
    int shiftAmount) {
  ShovelerImage* characterPngImage = shovelerImagePngReadFile(filename);
  ShovelerImage* characterAnimationTilesetImage =
      shovelerImageCreateAnimationTileset(characterPngImage, shiftAmount);
  GString* characterAnimationTilesetPngData = getImageData(characterAnimationTilesetImage);
  shovelerImageFree(characterPngImage);
  shovelerImageFree(characterAnimationTilesetImage);

  long long int tilesetEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
  ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, tilesetEntityId);
  entity->label = strdup("tileset");
  shovelerWorldEntityAddResourceComponent(
      entity,
      (const unsigned char*) characterAnimationTilesetPngData->str,
      (int) characterAnimationTilesetPngData->len);
  shovelerWorldEntityAddImageComponent(
      entity, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG, /* resourceEntityId */ 0);
  shovelerWorldEntityAddSamplerComponent(
      entity,
      /* interpolate */ true,
      /* useMipmaps */ false,
      /* clamp */ true);
  shovelerWorldEntityAddTextureImageComponent(entity, /* image */ 0);
  shovelerWorldEntityAddTilesetComponent(
      entity,
      /* image */ 0,
      /* columns */ 4,
      /* rows */ 3,
      /* padding */ 1);

  g_string_free(characterAnimationTilesetPngData, true);

  return tilesetEntityId;
}
