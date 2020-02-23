#ifndef SHOVELER_COMPONENT_TEXTURE_H
#define SHOVELER_COMPONENT_TEXTURE_H

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdTexture;

typedef enum {
	SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE,
} ShovelerComponentTextureOptionId;

ShovelerComponentType *shovelerComponentCreateTextureType();
ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component);

#endif
