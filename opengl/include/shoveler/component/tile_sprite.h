#ifndef SHOVELER_COMPONENT_TILE_SPRITE_H
#define SHOVELER_COMPONENT_TILE_SPRITE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

extern const char *const shovelerComponentTypeIdTileSprite;

typedef enum {
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_MATERIAL,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_COLUMN,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_ROW,
} ShovelerComponentTileSpriteOptionId;

ShovelerComponentType *shovelerComponentCreateTileSpriteType();
ShovelerSprite *shovelerComponentGetTileSprite(ShovelerComponent *component);

#endif
