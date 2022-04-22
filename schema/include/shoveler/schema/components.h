#ifndef SHOVELER_SCHEMA_COMPONENTS_H
#define SHOVELER_SCHEMA_COMPONENTS_H

#include <shoveler/types.h>
#include <shoveler/world.h>
#include <shoveler/schema/base.h>
#include <shoveler/schema/opengl.h>

void shovelerWorldEntityAddPositionAbsoluteComponent(ShovelerWorldEntity* entity, ShovelerVector3 positionCoordinates);
void shovelerWorldEntityAddDrawableComponent(ShovelerWorldEntity* entity, ShovelerComponentDrawableType drawableType);
void shovelerWorldEntityAddResourceComponent(ShovelerWorldEntity* entity, unsigned char* buffer, int bufferSize);
void shovelerWorldEntityAddImageComponent(ShovelerWorldEntity* entity, ShovelerComponentImageFormat format, long long int resource);
void shovelerWorldEntityAddSamplerComponent(ShovelerWorldEntity* entity, bool interpolate, bool useMipmaps, bool clamp);
void shovelerWorldEntityAddTextureImageComponent(ShovelerWorldEntity* entity, long long int image);
void shovelerWorldEntityAddTilesetComponent(ShovelerWorldEntity* entity, long long int image, int numColumns, int numRows, int padding);
void shovelerWorldEntityAddMaterialTileSpriteComponent(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddMaterialTilemapComponent(ShovelerWorldEntity* entity);
void shovelerWorldEntityAddMaterialCanvasComponent(ShovelerWorldEntity* entity, long long int canvas, ShovelerVector2 regionPosition, ShovelerVector2 regionSize);
void shovelerWorldEntityAddCanvasComponent(ShovelerWorldEntity* entity, int numLayers);
void shovelerWorldEntityAddTilemapCollidersComponent(ShovelerWorldEntity* entity, int numColumns, int numRows, unsigned char* colliders, int collidersSize);
void shovelerWorldEntityAddTilemapTilesDirectComponent(ShovelerWorldEntity* entity, int numColumns, int numRows, unsigned char* columns, unsigned char* rows, unsigned char* ids);
void shovelerWorldEntityAddTilemapComponent(ShovelerWorldEntity* entity, long long int tiles, long long int colliders, long long int* tilesets, int tilesetsSize);
void shovelerWorldEntityAddTilemapSpriteComponent(ShovelerWorldEntity* entity, long long int material, long long int tilemap);
void shovelerWorldEntityAddTileSpriteComponent(ShovelerWorldEntity* entity, long long int material, long long int tileset, int tilesetColumn, int tilesetRow);
void shovelerWorldEntityAddTileSpriteAnimationComponent(ShovelerWorldEntity* entity, long long int position, long long int tileSprite, ShovelerCoordinateMapping positionMappingX, ShovelerCoordinateMapping positionMappingY, float moveAmountThreshold);
void shovelerWorldEntityAddSpriteTilemapComponent(ShovelerWorldEntity* entity, long long int position, ShovelerCoordinateMapping positionMappingX, ShovelerCoordinateMapping positionMappingY, bool enableCollider, long long int canvas, int layer, ShovelerVector2 size, long long int tilemapSprite);
void shovelerWorldEntityAddSpriteTileComponent(ShovelerWorldEntity* entity, long long int position, ShovelerCoordinateMapping positionMappingX, ShovelerCoordinateMapping positionMappingY, bool enableCollider, long long int canvas, int layer, ShovelerVector2 size, long long int tileSprite);
void shovelerWorldEntityAddModelComponent(ShovelerWorldEntity* entity, long long int position, long long int drawable, long long int material, ShovelerVector3 rotation, ShovelerVector3 scale, bool visible, bool emitter, bool castsShadow, ShovelerComponentModelPolygonMode polygonMode);
void shovelerWorldEntityAddClientComponent(ShovelerWorldEntity* entity, long long int position);

#endif
