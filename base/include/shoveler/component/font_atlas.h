#ifndef SHOVELER_COMPONENT_FONT_ATLAS_H
#define SHOVELER_COMPONENT_FONT_ATLAS_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerFontAtlasStruct ShovelerFontAtlas; // forward declaration: font_atlas.h

extern const char *const shovelerComponentTypeIdFontAtlas;

typedef enum {
	SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT,
	SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT_SIZE,
	SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_PADDING,
} ShovelerComponentFontAtlasOptionId;

ShovelerComponentType *shovelerComponentCreateFontAtlasType();
ShovelerFontAtlas *shovelerComponentGetFontAtlas(ShovelerComponent *component);

#endif
