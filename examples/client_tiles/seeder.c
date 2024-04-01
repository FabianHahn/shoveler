#include "seeder.h"

#include <shoveler/file.h>
#include <shoveler/image.h>
#include <shoveler/image/png.h>
#include <shoveler/map.h>
#include <shoveler/schema/base.h>
#include <shoveler/schema/opengl.h>
#include <shoveler/world.h>
#include <stdlib.h>
#include <string.h>

#include "schema.h"

static GString* getImageData(ShovelerImage* image);
static long long int addCharacterAnimationTilesetEntity(
    ShovelerWorld* world, long long int* nextEntityId, const char* filename, int shiftAmount);

ShovelerClientTilesSeeder shovelerClientTilesSeederInit(
    ShovelerWorld* world,
    ShovelerMap* map,
    long long int* nextEntityId,
    const char* tilesetPngFilename,
    int tilesetPngColumns,
    int tilesetPngRows,
    const char* character1PngFilename,
    const char* character2PngFilename,
    const char* character3PngFilename,
    const char* character4PngFilename,
    int characterShiftAmount) {
  ShovelerClientTilesSeeder seeder;
  seeder.world = world;
  seeder.map = map;
  seeder.nextEntityId = nextEntityId;

  seeder.quadDrawableEntityId = *nextEntityId;
  {
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
    entity->label = strdup("quad drawable");
    shovelerWorldEntityAddDrawableQuad(entity);
  }

  seeder.tilesetEntityId = *nextEntityId;
  {
    ShovelerImage* tilesetPngImage = shovelerImagePngReadFile(tilesetPngFilename);
    GString* tilesetPngData = getImageData(tilesetPngImage);
    shovelerImageFree(tilesetPngImage);

    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
    entity->label = strdup("tileset");
    shovelerWorldEntityAddResource(
        entity, (const unsigned char*) tilesetPngData->str, (int) tilesetPngData->len);
    shovelerWorldEntityAddImagePng(entity, /* resourceEntityId */ 0);
    shovelerWorldEntityAddSampler(
        entity,
        /* interpolate */ true,
        /* useMipmaps */ false,
        /* clamp */ true);
    shovelerWorldEntityAddTextureImage(entity, /* image */ 0);
    shovelerWorldEntityAddTileset(
        entity,
        /* image */ 0,
        tilesetPngColumns,
        tilesetPngRows,
        /* padding */ 1);

    g_string_free(tilesetPngData, true);
  }

  seeder.characterTilesetEntityIds[0] = addCharacterAnimationTilesetEntity(
      world, nextEntityId, character1PngFilename, characterShiftAmount);
  seeder.characterTilesetEntityIds[1] = addCharacterAnimationTilesetEntity(
      world, nextEntityId, character2PngFilename, characterShiftAmount);
  seeder.characterTilesetEntityIds[2] = addCharacterAnimationTilesetEntity(
      world, nextEntityId, character3PngFilename, characterShiftAmount);
  seeder.characterTilesetEntityIds[3] = addCharacterAnimationTilesetEntity(
      world, nextEntityId, character4PngFilename, characterShiftAmount);

  seeder.canvasEntityId = *nextEntityId;
  {
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
    entity->label = strdup("canvas");
    shovelerWorldEntityAddMaterialTileSprite(entity);
    shovelerWorldEntityAddCanvas(entity, /* numLayers */ 3);
  }

  {
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
    entity->label = strdup("tile sprite material");
    shovelerWorldEntityAddMaterialTileSprite(entity);
  }

  seeder.tilemapMaterialEntityId = *nextEntityId;
  {
    ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
    entity->label = strdup("tilemap material");
    shovelerWorldEntityAddMaterialTilemap(entity);
  }

  ShovelerVector2 chunkSize2d =
      shovelerVector2((float) map->dimensions.chunkSize, (float) map->dimensions.chunkSize);
  ShovelerVector3 chunkScale3d = shovelerVector3(
      0.5f * (float) map->dimensions.chunkSize, 0.5f * (float) map->dimensions.chunkSize, 1.0f);
  for (int chunkX = 0; chunkX < map->dimensions.numChunkColumns; chunkX++) {
    for (int chunkY = 0; chunkY < map->dimensions.numChunkRows; chunkY++) {
      ShovelerMapChunk* chunk = shovelerMapGetChunk(map, chunkX, chunkY);

      long long int tilesets[] = {
          seeder.tilesetEntityId,
          /* no autumn/summer distinction here yet */ seeder.tilesetEntityId};
      {
        ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
        entity->label = strdup("chunk background");
        shovelerWorldEntityAddPosition(
            entity, chunk->position.values[0], chunk->position.values[1], 0.0f);
        shovelerWorldEntityAddTilemapColliders(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->backgroundTiles.tilesetColliders);
        shovelerWorldEntityAddTilemapTiles(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->backgroundTiles.tilesetColumns,
            chunk->backgroundTiles.tilesetRows,
            chunk->backgroundTiles.tilesetIds);
        shovelerWorldEntityAddTilemap(
            entity,
            /* tilesEntityId */ 0,
            /* collidersEntityId */ 0,
            tilesets,
            /* numTilesets */ 2);
        shovelerWorldEntityAddTilemapSprite(
            entity, seeder.tilemapMaterialEntityId, /* tilemapEntityId */ 0);
        shovelerWorldEntityAddSpriteTilemap(
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

      {
        ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
        entity->label = strdup("chunk foreground");
        shovelerWorldEntityAddPosition(
            entity, chunk->position.values[0], chunk->position.values[1], 0.0f);
        shovelerWorldEntityAddTilemapColliders(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->foregroundTiles.tilesetColliders);
        shovelerWorldEntityAddTilemapTiles(
            entity,
            /* numColumns */ map->dimensions.chunkSize,
            /* numRows */ map->dimensions.chunkSize,
            chunk->foregroundTiles.tilesetColumns,
            chunk->foregroundTiles.tilesetRows,
            chunk->foregroundTiles.tilesetIds);
        shovelerWorldEntityAddTilemap(
            entity,
            /* tilesEntityId */ 0,
            /* collidersEntityId */ 0,
            tilesets,
            /* numTilesets */ 2);
        shovelerWorldEntityAddTilemapSprite(
            entity, seeder.tilemapMaterialEntityId, /* tilemapEntityId */ 0);
        shovelerWorldEntityAddSpriteTilemap(
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
        ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
        entity->label = strdup("chunk foreground");
        shovelerWorldEntityAddPosition(
            entity, chunk->position.values[0], chunk->position.values[1], 0.0f);
        shovelerWorldEntityAddMaterialCanvas(
            entity, seeder.canvasEntityId, chunk->position, /* regionSize */ chunkSize2d);
        shovelerWorldEntityAddModel(
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

void shovelerClientTilesSeederSpawnPlayer(
    ShovelerClientTilesSeeder* seeder, ShovelerVector2 position) {
  ShovelerWorldEntity* entity = shovelerWorldAddEntity(seeder->world, (*seeder->nextEntityId)++);
  entity->label = strdup("player");
  shovelerWorldEntityAddPosition(entity, position.values[0], position.values[1], 5.0f);
  shovelerWorldEntityDelegateComponent(entity, shovelerComponentTypeIdPosition);
  shovelerWorldEntityAddClient(entity, /* position */ 0);
  shovelerWorldEntityAddTileSprite(
      entity,
      /* materialEntityId */ seeder->canvasEntityId,
      seeder->characterTilesetEntityIds[seeder->nextPlayerTilesetIndex],
      /* tilesetColumn */ 0,
      /* tilesetRow */ 0);
  shovelerWorldEntityAddTileSpriteAnimation(
      entity,
      /* position */ 0,
      /* tileSprite */ 0,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
      SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
      /* moveAmountThreshold */ 0.5f);
  shovelerWorldEntityAddSpriteTile(
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
    ShovelerWorld* world, long long int* nextEntityId, const char* filename, int shiftAmount) {
  ShovelerImage* characterPngImage = shovelerImagePngReadFile(filename);
  ShovelerImage* characterAnimationTilesetImage =
      shovelerImageCreateAnimationTileset(characterPngImage, shiftAmount);
  GString* characterAnimationTilesetPngData = getImageData(characterAnimationTilesetImage);
  shovelerImageFree(characterPngImage);
  shovelerImageFree(characterAnimationTilesetImage);

  long long int tilesetEntityId = *nextEntityId;
  ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, (*nextEntityId)++);
  entity->label = strdup("tileset");
  shovelerWorldEntityAddResource(
      entity,
      (const unsigned char*) characterAnimationTilesetPngData->str,
      (int) characterAnimationTilesetPngData->len);
  shovelerWorldEntityAddImagePng(entity, /* resourceEntityId */ 0);
  shovelerWorldEntityAddSampler(
      entity,
      /* interpolate */ true,
      /* useMipmaps */ false,
      /* clamp */ true);
  shovelerWorldEntityAddTextureImage(entity, /* image */ 0);
  shovelerWorldEntityAddTileset(
      entity,
      /* image */ 0,
      /* columns */ 4,
      /* rows */ 3,
      /* padding */ 1);

  g_string_free(characterAnimationTilesetPngData, true);

  return tilesetEntityId;
}
