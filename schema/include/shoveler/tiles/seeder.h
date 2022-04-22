#ifndef SHOVELER_TILES_SEEDER_H
#define SHOVELER_TILES_SEEDER_H

#include <shoveler/world.h>
#include <shoveler/entity_id_allocator.h>
#include <shoveler/map.h>

typedef struct {
	long long int tilesetEntityId;
	long long int tilesetPngEntityId;
	long long int tilemapMaterialEntityId;
	long long int canvasEntityId;
	long long int quadDrawableEntityId;
} ShovelerTilesSeedBase;

typedef struct {
	long long int backgroundEntityId;
	long long int foregroundEntityId;
	long long int chunkEntityId;
} ShovelerTilesSeedChunk;

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
	int characterShiftAmount);
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
	int characterShiftAmount);
ShovelerTilesSeedChunk shovelerTilesSeedChunk(
	ShovelerWorld* world,
	ShovelerEntityIdAllocator* entityIdAllocator,
	ShovelerMapChunk* chunk,
	ShovelerTilesSeedBase seedBase);

#endif
