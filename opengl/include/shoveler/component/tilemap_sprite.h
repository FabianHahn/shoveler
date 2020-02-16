#ifndef SHOVELER_COMPONENT_TILEMAP_SPRITE_H
#define SHOVELER_COMPONENT_TILEMAP_SPRITE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

extern const char *const shovelerComponentTypeIdTilemapSprite;

typedef enum {
	SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_MATERIAL,
	SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_TILEMAP,
} ShovelerComponentTilemapSpriteOptionId;

ShovelerComponentType *shovelerComponentCreateTilemapSpriteType();
ShovelerSprite *shovelerComponentGetTilemapSprite(ShovelerComponent *component);

#endif
