#ifndef SHOVELER_SCHEMA_COMPONENTS_H
#define SHOVELER_SCHEMA_COMPONENTS_H

#include <shoveler/schema/base.h>
#include <shoveler/schema/opengl.h>
#include <shoveler/types.h>
#include <shoveler/world.h>

void shovelerWorldEntityAddPositionAbsoluteComponent(
    ShovelerWorldEntity* entity, ShovelerVector3 positionCoordinates);
void shovelerWorldEntityAddDrawableComponent(
    ShovelerWorldEntity* entity, ShovelerComponentDrawableType drawableType);
void shovelerWorldEntityAddResourceComponent(
    ShovelerWorldEntity* entity, const unsigned char* buffer, int bufferSize);
void shovelerWorldEntityAddImageComponent(
    ShovelerWorldEntity* entity,
    ShovelerComponentImageFormat format,
    long long int resourceEntityId);
void shovelerWorldEntityAddSamplerComponent(
    ShovelerWorldEntity* entity, bool interpolate, bool useMipmaps, bool clamp);
void shovelerWorldEntityAddTextureImageComponent(
    ShovelerWorldEntity* entity, long long int imageEntityId);
void shovelerWorldEntityAddTilesetComponent(
    ShovelerWorldEntity* entity,
    long long int imageEntityId,
    int numColumns,
    int numRows,
    int padding);
void shovelerWorldEntityAddMaterialCanvasComponent(
    ShovelerWorldEntity* entity,
    long long int canvasEntityId,
    ShovelerVector2 position,
    ShovelerVector2 regionSize);
void shovelerWorldEntityAddMaterialTileSpriteComponent(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddMaterialTilemapComponent(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddCanvasComponent(ShovelerWorldEntity* entity, int numLayers);
void shovelerWorldEntityAddTilemapCollidersComponent(
    ShovelerWorldEntity* entity,
    int numColumns,
    int numRows,
    const unsigned char* colliders,
    int collidersSize);
void shovelerWorldEntityAddTilemapTilesDirectComponent(
    ShovelerWorldEntity* entity,
    int numColumns,
    int numRows,
    const unsigned char* columns,
    const unsigned char* rows,
    const unsigned char* ids);
void shovelerWorldEntityAddTilemapComponent(
    ShovelerWorldEntity* entity,
    long long int tilesEntityId,
    long long int collidersEntityId,
    const long long int* tilesetsEntityIds,
    int tilesetsSize);
void shovelerWorldEntityAddTilemapSpriteComponent(
    ShovelerWorldEntity* entity, long long int materialEntityId, long long int tilemapEntityId);
void shovelerWorldEntityAddTileSpriteComponent(
    ShovelerWorldEntity* entity,
    long long int materialEntityId,
    long long int EntityId,
    int tilesetColumn,
    int tilesetRow);
void shovelerWorldEntityAddTileSpriteAnimationComponent(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    long long int tileSpriteEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    float moveAmountThreshold);
void shovelerWorldEntityAddSpriteTilemapComponent(
    ShovelerWorldEntity* entity,
    long long int positionEntityId,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvasEntityId,
    int layer,
    ShovelerVector2 size,
    long long int tilemapSpriteEntityId);
void shovelerWorldEntityAddSpriteTileComponent(
    ShovelerWorldEntity* entity,
    long long int position,
    ShovelerCoordinateMapping positionMappingX,
    ShovelerCoordinateMapping positionMappingY,
    bool enableCollider,
    long long int canvasEntityId,
    int layer,
    ShovelerVector2 size,
    long long int tileSpriteEntityId);
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
    ShovelerComponentModelPolygonMode polygonMode);
void shovelerWorldEntityAddClientComponent(
    ShovelerWorldEntity* entity, long long int positionEntityId);

#endif
