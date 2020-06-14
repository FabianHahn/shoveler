#ifndef SHOVELER_COMPONENT_TEXTURE_SPRITE_H
#define SHOVELER_COMPONENT_TEXTURE_SPRITE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

extern const char *const shovelerComponentTypeIdTextureSprite;

typedef enum {
	SHOVELER_COMPONENT_TEXTURE_SPRITE_OPTION_ID_MATERIAL,
	SHOVELER_COMPONENT_TEXTURE_SPRITE_OPTION_ID_TEXTURE,
} ShovelerComponentTextureSpriteOptionId;

ShovelerComponentType *shovelerComponentCreateTextureSpriteType();
ShovelerSprite *shovelerComponentGetTextureSprite(ShovelerComponent *component);

#endif
