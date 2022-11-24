#ifndef SHOVELER_EXAMPLES_CLIENT_TILES_SEEDER_H
#define SHOVELER_EXAMPLES_CLIENT_TILES_SEEDER_H

typedef struct ShovelerMapStruct ShovelerMap;
typedef struct ShovelerWorldStruct ShovelerWorld;

void shovelerClientTilesSeederInit(
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
    int characterShiftAmount);

#endif
