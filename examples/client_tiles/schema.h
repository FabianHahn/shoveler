#ifndef SHOVELER_EXAMPLES_CLIENT_TILES_SCHEMA_H
#define SHOVELER_EXAMPLES_CLIENT_TILES_SCHEMA_H

#include <shoveler/schema/opengl.h>
#include <shoveler/types.h>
#include <stdbool.h>

typedef struct ShovelerWorldEntityStruct ShovelerWorldEntity;

void shovelerWorldEntityAddDrawableQuad(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddResource(
    ShovelerWorldEntity* entity, const unsigned char* data, int size);
void shovelerWorldEntityAddImagePng(ShovelerWorldEntity* entity, long long int resourceEntityId);
void shovelerWorldEntityAddSampler(
    ShovelerWorldEntity* entity, bool interpolate, bool useMipmaps, bool clamp);
void shovelerWorldEntityAddTextureImage(ShovelerWorldEntity* entity, long long int imageEntityId);
void shovelerWorldEntityAddTileset(
    ShovelerWorldEntity* entity,
    long long int imageEntityId,
    int numColumns,
    int numRows,
    int padding);
void shovelerWorldEntityAddMaterialCanvas(
    ShovelerWorldEntity* entity,
    long long int canvasEntityId,
    ShovelerVector2 position,
    ShovelerVector2 regionSize);
void shovelerWorldEntityAddMaterialTilemap(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddMaterialTileSprite(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddCanvas(ShovelerWorldEntity* entity, int numLayers);
void shovelerWorldEntityAddTilemapColliders(
    ShovelerWorldEntity* entity, int numColumns, int numRows, const unsigned char* colliders);
void shovelerWorldEntityAddTilemapTiles(
    ShovelerWorldEntity* entity,
    int numColumns,
    int numRows,
    const unsigned char* columns,
    const unsigned char* rows,
    const unsigned char* ids);
void shovelerWorldEntityAddTilemap(
    ShovelerWorldEntity* entity,
    long long int tilesEntityId,
    long long int colliderEntityId,
    long long int* tilesets,
    int numTilesets);
void shovelerWorldEntityAddTileSprite(
    ShovelerWorldEntity* entity,
    long long int materialEntityId,
    long long int tilesetEntityId,
    int tilesetColumn,
    int tilesetRow);
void shovelerWorldEntityAddTileSpriteAnimation(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    long long int tileSpriteEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    float moveAmountThreshold);
void shovelerWorldEntityAddTilemapSprite(
    ShovelerWorldEntity* entity, long long int materialEntityId, long long int tilemapEntityId);
void shovelerWorldEntityAddSpriteTile(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvasEntityId,
    int layer,
    ShovelerVector2 size,
    long long int tileSpriteEntityId);
void shovelerWorldEntityAddSpriteTilemap(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvasEntityId,
    int layer,
    ShovelerVector2 size,
    long long int tilemapSpriteEntityId);
void shovelerWorldEntityAddModel(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    long long int drawableEntityId,
    long long int materialEntityId,
    ShovelerVector3 rotation,
    ShovelerVector3 scale,
    bool visible,
    bool emitter,
    bool castsShadow,
    ShovelerComponentModelPolygonMode polygonMode);
void shovelerWorldEntityAddClient(ShovelerWorldEntity* entity, long long int positionEntityId);
void shovelerWorldEntityAddPosition(ShovelerWorldEntity* entity, float x, float y, float z);

#endif
