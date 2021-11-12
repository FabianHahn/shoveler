#include "shoveler/component/fonts.h"

#include "shoveler/component.h"
#include "shoveler/font.h"

const char *const shovelerComponentViewTargetIdFonts = "fonts";

ShovelerFonts *shovelerComponentGetViewFonts(ShovelerComponent *component)
{
	return (ShovelerFonts *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdFonts);
}
