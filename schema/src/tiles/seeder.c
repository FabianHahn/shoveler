#include "shoveler/tiles/seeder.h"

#include <string.h>
#include <stdlib.h>
#include "shoveler/entity_id_allocator.h"
#include "shoveler/file.h"
#include "shoveler/image/png.h"
#include "shoveler/map.h"
#include "shoveler/schema/components.h"
#include "shoveler/tiles/tileset.h"

static const int chunkSize = 10;

static GString* getImageData(ShovelerImage* image);
static bool addCharacterAnimationTilesetEntity(
	ShovelerWorld* world,
	ShovelerEntityIdAllocator* entityIdAllocator,
	const char* filename,
	int shiftAmount);

void shovelerTilesSeedMap(
	ShovelerWorld* world,
	ShovelerEntityIdAllocator* entityIdAllocator,
	ShovelerMap* map,
	const char* tilesetPngFilename,
	int tilesetPngColumns,
	int tilesetPngRows,
	const char* character1PngFilename,
	const char* character2PngFilename,
	const char* character3PngFilename,
	const char* character4PngFilename,
	int characterShiftAmount)
{
	ShovelerTilesSeedBase seedBase = shovelerTilesSeedBase(
		world,
		entityIdAllocator,
		tilesetPngFilename,
		tilesetPngColumns,
		tilesetPngRows,
		character1PngFilename,
		character2PngFilename,
		character3PngFilename,
		character4PngFilename,
		characterShiftAmount);

	for (int i = 0; i < map->dimensions.numChunkColumns * map->dimensions.numChunkRows; i++) {
		ShovelerMapChunk* chunk = &map->chunks[i];

		shovelerTilesSeedChunk(
			world,
			entityIdAllocator,
			chunk,
			seedBase);
	}
}

ShovelerTilesSeedBase shovelerTilesSeedBase(
	ShovelerWorld* world,
	ShovelerEntityIdAllocator* entityIdAllocator,
	const char* tilesetPngFilename,
	int tilesetPngColumns,
	int tilesetPngRows,
	const char* character1PngFilename,
	const char* character2PngFilename,
	const char* character3PngFilename,
	const char* character4PngFilename,
	int characterShiftAmount)
{
	ShovelerTilesSeedBase seedBase;

	{ // bootstrap
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(
			world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
		entity->label = strdup("bootstrap");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
	}

	seedBase.quadDrawableEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // quad drawable
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedBase.quadDrawableEntityId);
		entity->label = strdup("drawable");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
		shovelerWorldEntityAddDrawableComponent(entity, SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD);
	}

	seedBase.tilesetEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // tileset
		ShovelerImage* tilesetImage;
		int tilesetColumns;
		int tilesetRows;
		shovelerTilesCreateTileset(&tilesetImage, &tilesetColumns, &tilesetRows);
		GString* imageData = getImageData(tilesetImage);
		shovelerImageFree(tilesetImage);

		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedBase.tilesetEntityId);
		entity->label = strdup("tileset");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
		shovelerWorldEntityAddResourceComponent(entity, (unsigned char*) imageData->str, imageData->len);
		shovelerWorldEntityAddImageComponent(entity, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG, /* resource */ 0);
		shovelerWorldEntityAddSamplerComponent(entity, /* interpolate */ true, /* useMipmaps */ false, /* clamp */ true);
		shovelerWorldEntityAddTextureImageComponent(entity, /* image */ 0);
		shovelerWorldEntityAddTilesetComponent(entity, /* image */ 0, tilesetColumns, tilesetRows, /* padding */ 1);

		g_string_free(imageData, true);
	}

	seedBase.tilesetPngEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // tileset png
		ShovelerImage* tilesetPngImage = shovelerImagePngReadFile(tilesetPngFilename);
		GString* tilesetPngData = getImageData(tilesetPngImage);
		shovelerImageFree(tilesetPngImage);

		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedBase.tilesetPngEntityId);
		entity->label = strdup("tileset");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
		shovelerWorldEntityAddResourceComponent(entity, (unsigned char*) tilesetPngData->str, tilesetPngData->len);
		shovelerWorldEntityAddImageComponent(entity, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG, /* resource */ 0);
		shovelerWorldEntityAddSamplerComponent(entity, /* interpolate */ true, /* useMipmaps */ false, /* clamp */ true);
		shovelerWorldEntityAddTextureImageComponent(entity, /* image */ 0);
		shovelerWorldEntityAddTilesetComponent(entity, /* image */ 0, tilesetPngColumns, tilesetPngRows, /* padding */ 1);

		g_string_free(tilesetPngData, true);
	}

	// character 1 animation tileset
	addCharacterAnimationTilesetEntity(
		world, entityIdAllocator, character1PngFilename, characterShiftAmount);

	// character 2 animation tileset
	addCharacterAnimationTilesetEntity(
		world, entityIdAllocator, character2PngFilename, characterShiftAmount);

	// character 3 animation tileset
	addCharacterAnimationTilesetEntity(
		world, entityIdAllocator, character3PngFilename, characterShiftAmount);

	// character 4 animation tileset
	addCharacterAnimationTilesetEntity(
		world, entityIdAllocator, character4PngFilename, characterShiftAmount);

	seedBase.canvasEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // canvas
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedBase.canvasEntityId);
		entity->label = strdup("canvas");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
		shovelerWorldEntityAddMaterialTileSpriteComponent(entity);
		shovelerWorldEntityAddCanvasComponent(entity, /* numLayers */ 3);
	}

	{ // tile sprite material
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(
			world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
		entity->label = strdup("material");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
		shovelerWorldEntityAddMaterialTileSpriteComponent(entity);
	}

	seedBase.tilemapMaterialEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // tilemap material
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedBase.tilemapMaterialEntityId);
		entity->label = strdup("material");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
		shovelerWorldEntityAddMaterialTilemapComponent(entity);
	}

	return seedBase;
}

ShovelerTilesSeedChunk shovelerTilesSeedChunk(
	ShovelerWorld* world,
	ShovelerEntityIdAllocator* entityIdAllocator,
	ShovelerMapChunk* chunk,
	ShovelerTilesSeedBase seedBase)
{
	ShovelerTilesSeedChunk seedChunk;

	ShovelerVector3 chunkPosition = shovelerVector3(
		chunk->position.values[0], chunk->position.values[1], 0.0f);

	seedChunk.backgroundEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // chunk background
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedChunk.backgroundEntityId);
		entity->label = strdup("chunk_background");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, chunkPosition);
		shovelerWorldEntityAddTilemapCollidersComponent(
			entity,
			/* numColumns */ chunkSize,
			/* numRows */ chunkSize,
			chunk->backgroundTiles.tilesetColliders,
			/* collidersSize */ chunkSize * chunkSize);
		shovelerWorldEntityAddTilemapTilesDirectComponent(
			entity,
			/* numColums */ chunkSize,
			/* numRows */ chunkSize,
			chunk->backgroundTiles.tilesetColumns,
			chunk->backgroundTiles.tilesetRows,
			chunk->backgroundTiles.tilesetIds);
		shovelerWorldEntityAddTilemapComponent(
			entity,
			/* tiles */ 0,
			/* collider */ 0,
			/* tilesets */ (long long int[]) {seedBase.tilesetEntityId, seedBase.tilesetPngEntityId},
			/* numTilesets */ 2);
		shovelerWorldEntityAddTilemapSpriteComponent(entity, seedBase.tilemapMaterialEntityId, /* tilemap */ 0);
		shovelerWorldEntityAddSpriteTilemapComponent(
			entity,
			/* position */ 0,
			SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
			SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
			/* enableCollider */ true,
			seedBase.canvasEntityId,
			/* layer */ 0,
			/* size */ shovelerVector2((float) chunkSize, (float) chunkSize),
			/* tilemapSprite */ 0);
	}

	seedChunk.foregroundEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // chunk foreground
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedChunk.foregroundEntityId);
		entity->label = strdup("chunk_foreground");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, chunkPosition);
		shovelerWorldEntityAddTilemapCollidersComponent(
			entity,
			/* numColums */ chunkSize,
			/* numRows */ chunkSize,
			chunk->foregroundTiles.tilesetColliders,
			/* collidersSize */ chunkSize * chunkSize);
		shovelerWorldEntityAddTilemapTilesDirectComponent(
			entity,
			/* numColums */ chunkSize,
			/* numRows */ chunkSize,
			chunk->foregroundTiles.tilesetColumns,
			chunk->foregroundTiles.tilesetRows,
			chunk->foregroundTiles.tilesetIds);
		shovelerWorldEntityAddTilemapComponent(
			entity,
			/* tiles */ 0,
			/* collider */ 0,
			/* tilesets */ (long long int[]) {seedBase.tilesetEntityId, seedBase.tilesetPngEntityId},
			/* numTilesets */ 2);
		shovelerWorldEntityAddTilemapSpriteComponent(
			entity, seedBase.tilemapMaterialEntityId, /* tilemap */ 0);
		shovelerWorldEntityAddSpriteTilemapComponent(
			entity,
			/* position */ 0,
			SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
			SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
			/* enableCollider */ true,
			seedBase.canvasEntityId,
			/* layer */ 2,
			/* size */ shovelerVector2((float) chunkSize, (float) chunkSize),
			/* tilemapSprite */ 0);
	}

	seedChunk.chunkEntityId = shovelerEntityIdAllocatorAllocate(entityIdAllocator);
	{ // chunk
		ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, seedChunk.chunkEntityId);
		entity->label = strdup("chunk_foreground");
		shovelerWorldEntityAddPositionAbsoluteComponent(entity, chunkPosition);
		shovelerWorldEntityAddMaterialCanvasComponent(
			entity,
			seedBase.canvasEntityId,
			chunk->position,
			/* regionSize */ shovelerVector2((float) chunkSize, (float) chunkSize));
		shovelerWorldEntityAddModelComponent(
			entity,
			/* position */ 0,
			seedBase.quadDrawableEntityId,
			/* material */ 0,
			/* rotation */ shovelerVector3(0.0f, 0.0f, 0.0f),
			/* scale */ shovelerVector3(0.5f * (float) chunkSize, 0.5f * (float) chunkSize, 1.0f),
			/* visible */ true,
			/* emitter */ true,
			/* castsShadow */ false,
			SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	}

	return seedChunk;
}

static GString* getImageData(ShovelerImage* image)
{
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

static bool addCharacterAnimationTilesetEntity(
	ShovelerWorld* world,
	ShovelerEntityIdAllocator* entityIdAllocator,
	const char* filename,
	int shiftAmount)
{
	ShovelerImage* characterPngImage = shovelerImagePngReadFile(filename);
	ShovelerImage* characterAnimationTilesetImage = shovelerImageCreateAnimationTileset(characterPngImage, shiftAmount);
	GString* characterAnimationTilesetPngData = getImageData(characterAnimationTilesetImage);
	shovelerImageFree(characterPngImage);
	shovelerImageFree(characterAnimationTilesetImage);

	ShovelerWorldEntity* entity = shovelerWorldAddEntity(world, shovelerEntityIdAllocatorAllocate(entityIdAllocator));
	entity->label = strdup("tileset");
	shovelerWorldEntityAddPositionAbsoluteComponent(entity, shovelerVector3(-100.0f, -100.0f, -100.0f));
	shovelerWorldEntityAddResourceComponent(entity, (unsigned char*) characterAnimationTilesetPngData->str, characterAnimationTilesetPngData->len);
	shovelerWorldEntityAddImageComponent(entity, SHOVELER_COMPONENT_IMAGE_FORMAT_PNG, /* resource */ 0);
	shovelerWorldEntityAddSamplerComponent(entity, /* interpolate */ true, /* useMipmaps */ false, /* clamp */ true);
	shovelerWorldEntityAddTextureImageComponent(entity, /* image */ 0);
	shovelerWorldEntityAddTilesetComponent(entity, /* image */ 0, /* columns */ 4, /* rows */ 3, /* padding */ 1);

	g_string_free(characterAnimationTilesetPngData, true);

	return true;
}
