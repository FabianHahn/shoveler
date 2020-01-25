#ifndef SHOVELER_COMPONENT_TILE_SPRITE_H
#define SHOVELER_COMPONENT_TILE_SPRITE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

extern const char *const shovelerComponentTypeIdTileSprite;

typedef enum {
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_MATERIAL,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_COLUMN,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_ROW,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_X,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_Y,
	SHOVELER_COMPONENT_TILE_SPRITE_OPTION_SIZE,
} ShovelerComponentTileSpriteOptionId;

ShovelerComponentType *shovelerComponentCreateTileSpriteType();
ShovelerSprite *shovelerComponentGetTileSprite(ShovelerComponent *component);

#endif
