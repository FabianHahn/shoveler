#ifndef SHOVELER_COMPONENT_FONT_H
#define SHOVELER_COMPONENT_FONT_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerFontStruct ShovelerFont; // forward declaration: font.h

extern const char *const shovelerComponentTypeIdFont;

typedef enum {
	SHOVELER_COMPONENT_FONT_OPTION_ID_NAME,
	SHOVELER_COMPONENT_FONT_OPTION_ID_RESOURCE,
} ShovelerComponentFontOptionId;

ShovelerComponentType *shovelerComponentCreateFontType();
ShovelerFont *shovelerComponentGetFont(ShovelerComponent *component);

#endif
