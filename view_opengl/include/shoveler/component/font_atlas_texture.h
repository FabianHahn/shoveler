#ifndef SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_H
#define SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerFontAtlasTextureStruct ShovelerFontAtlasTexture; // forward declaration: font_atlas_texture.h

extern const char *const shovelerComponentTypeIdFontAtlasTexture;

typedef enum {
	SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_OPTION_ID_FONT_ATLAS,
} ShovelerComponentFontAtlasTextureOptionId;

ShovelerComponentType *shovelerComponentCreateFontAtlasTextureType();
ShovelerFontAtlasTexture *shovelerComponentGetFontAtlasTexture(ShovelerComponent *component);

#endif
