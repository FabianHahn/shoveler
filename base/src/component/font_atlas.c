#include "shoveler/component/font_atlas.h"

#include "shoveler/component/font.h"
#include "shoveler/component.h"
#include "shoveler/font_atlas.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdFontAtlas = "font_atlas";

static void *activateFontAtlasComponent(ShovelerComponent *component);
static void deactivateFontAtlasComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateFontAtlasType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT] = shovelerComponentTypeConfigurationOptionDependency("font", shovelerComponentTypeIdFont, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT_SIZE] = shovelerComponentTypeConfigurationOption("font_size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_PADDING] = shovelerComponentTypeConfigurationOption("name", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdFontAtlas, activateFontAtlasComponent, /* update */ NULL, deactivateFontAtlasComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerFontAtlas *shovelerComponentGetFontAtlas(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdFontAtlas);

	return component->data;
}

static void *activateFontAtlasComponent(ShovelerComponent *component)
{
	ShovelerComponent *fontComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT);
	assert(fontComponent != NULL);
	ShovelerFont *font = shovelerComponentGetFont(fontComponent);
	assert(font != NULL);

	int fontSize = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT_SIZE);
	int padding = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_PADDING);

	return shovelerFontAtlasCreate(font, fontSize, padding);
}

static void deactivateFontAtlasComponent(ShovelerComponent *component)
{
	ShovelerFontAtlas *fontAtlas = component->data;

	shovelerFontAtlasFree(fontAtlas);
}
