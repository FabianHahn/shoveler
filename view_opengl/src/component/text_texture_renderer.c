#include "shoveler/component/text_texture_renderer.h"

#include "shoveler/component/font_atlas_texture.h"
#include "shoveler/component/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/text_texture_renderer.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdTextTextureRenderer = "text_texture_renderer";

static void *activateTextTextureRendererComponent(ShovelerComponent *component);
static void deactivateTextTextureRendererComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextTextureRendererType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_OPTION_ID_FONT_ATLAS_TEXTURE] = shovelerComponentTypeConfigurationOptionDependency("font_atlas", shovelerComponentTypeIdFontAtlasTexture, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTextTextureRenderer, activateTextTextureRendererComponent, /* update */ NULL, deactivateTextTextureRendererComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTextTextureRenderer *shovelerComponentGetTextTextureRenderer(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTextTextureRenderer);

	return component->data;
}

static void *activateTextTextureRendererComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerComponent *fontAtlasTextureComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_OPTION_ID_FONT_ATLAS_TEXTURE);
	assert(fontAtlasTextureComponent != NULL);
	ShovelerFontAtlasTexture *fontAtlasTexture = shovelerComponentGetFontAtlasTexture(fontAtlasTextureComponent);
	assert(fontAtlasTexture != NULL);

	ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);

	return shovelerTextTextureRendererCreate(fontAtlasTexture, shaderCache);
}

static void deactivateTextTextureRendererComponent(ShovelerComponent *component)
{
	ShovelerTextTextureRenderer *textTextureRenderer = component->data;

	shovelerTextTextureRendererFree(textTextureRenderer);
}
