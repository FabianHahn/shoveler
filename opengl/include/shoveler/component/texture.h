#ifndef SHOVELER_COMPONENT_TEXTURE_H
#define SHOVELER_COMPONENT_TEXTURE_H

typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTextureComponentTypeName = "texture";
static const char *shovelerViewTextureImageResourceOptionKey = "image_resource";

ShovelerComponentType *shovelerComponentCreateTextureType();
ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component);

#endif
