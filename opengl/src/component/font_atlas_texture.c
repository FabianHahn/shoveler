#include "shoveler/component/font_atlas_texture.h"

#include "shoveler/component/font_atlas.h"
#include "shoveler/component.h"
#include "shoveler/font_atlas_texture.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdFontAtlasTexture = "font_atlas_texture";

static void *activateFontAtlasTextureComponent(ShovelerComponent *component);
static void deactivateFontAtlasTextureComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateFontAtlasTextureType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_OPTION_ID_FONT_ATLAS] = shovelerComponentTypeConfigurationOptionDependency("font_atlas", shovelerComponentTypeIdFontAtlas, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdFontAtlasTexture, activateFontAtlasTextureComponent, /* update */ NULL, deactivateFontAtlasTextureComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerFontAtlasTexture *shovelerComponentGetFontAtlasTexture(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdFontAtlasTexture);

	return component->data;
}

static void *activateFontAtlasTextureComponent(ShovelerComponent *component)
{
	ShovelerComponent *fontAtlasComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_OPTION_ID_FONT_ATLAS);
	assert(fontAtlasComponent != NULL);
	ShovelerFontAtlas *fontAtlas = shovelerComponentGetFontAtlas(fontAtlasComponent);
	assert(fontAtlas != NULL);

	ShovelerFontAtlasTexture *fontAtlasTexture = shovelerFontAtlasTextureCreate(fontAtlas);
	shovelerFontAtlasTextureUpdate(fontAtlasTexture);

	return fontAtlasTexture;
}

static void deactivateFontAtlasTextureComponent(ShovelerComponent *component)
{
	ShovelerFontAtlasTexture *fontAtlasTexture = component->data;

	shovelerFontAtlasTextureFree(fontAtlasTexture);
}
