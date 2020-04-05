#ifndef SHOVELER_COMPONENT_FONTS_H
#define SHOVELER_COMPONENT_FONTS_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerFontsStruct ShovelerFonts; // forward declaration: fonts.h

extern const char *const shovelerComponentViewTargetIdFonts;

ShovelerFonts *shovelerComponentGetViewFonts(ShovelerComponent *component);

static inline bool shovelerComponentHasViewFonts(ShovelerComponent *component)
{
	return shovelerComponentGetViewFonts(component) != NULL;
}

#endif
