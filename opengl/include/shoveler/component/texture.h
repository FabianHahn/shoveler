#ifndef SHOVELER_COMPONENT_TEXTURE_H
#define SHOVELER_COMPONENT_TEXTURE_H

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTexture;

typedef enum {
	SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TYPE,
	SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE,
	SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT_TEXTURE_RENDERER,
	SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT,
} ShovelerComponentTextureOptionId;

typedef enum {
	SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE,
	SHOVELER_COMPONENT_TEXTURE_TYPE_TEXT,
} ShovelerComponentTextureType;

ShovelerComponentType *shovelerComponentCreateTextureType();
ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component);

#endif
