#ifndef SHOVELER_COMPONENT_TEXT_SPRITE_H
#define SHOVELER_COMPONENT_TEXT_SPRITE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

extern const char *const shovelerComponentTypeIdTextSprite;

typedef enum {
	SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_MATERIAL,
	SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_FONT_ATLAS,
	SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_FONT_SIZE,
	SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_CONTENT,
	SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_COLOR,
} ShovelerComponentTextSpriteOptionId;

ShovelerComponentType *shovelerComponentCreateTextSpriteType();
ShovelerSprite *shovelerComponentGetTextSprite(ShovelerComponent *component);

#endif
