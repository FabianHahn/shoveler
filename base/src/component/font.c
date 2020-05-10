#include "shoveler/component/font.h"

#include "shoveler/component/fonts.h"
#include "shoveler/component/resource.h"
#include "shoveler/component.h"
#include "shoveler/font.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdFont = "font";

static void *activateFontComponent(ShovelerComponent *component);
static void deactivateFontComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateFontType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[2];
	configurationOptions[SHOVELER_COMPONENT_FONT_OPTION_ID_NAME] = shovelerComponentTypeConfigurationOption("name", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_FONT_OPTION_ID_RESOURCE] = shovelerComponentTypeConfigurationOptionDependency("resource", shovelerComponentTypeIdResource, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdFont, activateFontComponent, /* update */ NULL, deactivateFontComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerFont *shovelerComponentGetFont(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdFont);

	return component->data;
}

static void *activateFontComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewFonts(component));

	ShovelerComponent *resourceComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_FONT_OPTION_ID_RESOURCE);
	assert(resourceComponent != NULL);

	const char *name = shovelerComponentGetConfigurationValueString(component, SHOVELER_COMPONENT_FONT_OPTION_ID_NAME);

	const unsigned char *bufferData;
	int bufferSize;
	shovelerComponentGetResource(resourceComponent, &bufferData, &bufferSize);

	ShovelerFonts *fonts = shovelerComponentGetViewFonts(component);
	ShovelerFont *font = shovelerFontsLoadFontBuffer(fonts, name, bufferData, bufferSize);
	return font;
}

static void deactivateFontComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewFonts(component));

	ShovelerFont *font = component->data;

	ShovelerFonts *fonts = shovelerComponentGetViewFonts(component);
	shovelerFontsUnloadFont(fonts, font->name);
}
