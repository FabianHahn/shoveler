#include "shoveler/component/texture.h"

#include "shoveler/component/image.h"
#include "shoveler/component/render_state.h"
#include "shoveler/component/text_texture_renderer.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"
#include "shoveler/text_texture_renderer.h"

const char *const shovelerComponentTypeIdTexture = "texture";

static void *activateTextureComponent(ShovelerComponent *component);
static void deactivateTextureComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextureType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[4];
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE] = shovelerComponentTypeConfigurationOptionDependency("image", shovelerComponentTypeIdImage, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT_TEXTURE_RENDERER] = shovelerComponentTypeConfigurationOptionDependency("text_texture_renderer", shovelerComponentTypeIdTextTextureRenderer, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT] = shovelerComponentTypeConfigurationOption("text", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ true, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTexture, activateTextureComponent, /* update */ NULL, deactivateTextureComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTexture *shovelerComponentGetTexture(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTexture);

	return component->data;
}

static void *activateTextureComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewRenderState(component));

	ShovelerTexture *texture;

	ShovelerComponentTextureType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TYPE);
	switch(type) {
		case SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE: {
			if(!shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE)) {
				shovelerLogWarning("Failed to activate texture component of entity %lld: No image dependency specified", component->entityId);
				return NULL;
			}

			ShovelerComponent *imageComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_IMAGE);
			assert(imageComponent != NULL);
			ShovelerImage *image = shovelerComponentGetImage(imageComponent);
			assert(image != NULL);

			texture = shovelerTextureCreate2d(image, false);
			shovelerTextureUpdate(texture);
		} break;
		case SHOVELER_COMPONENT_TEXTURE_TYPE_TEXT: {
			if(!shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT_TEXTURE_RENDERER)) {
				shovelerLogWarning("Failed to activate texture component of entity %lld: No text texture renderer dependency specified", component->entityId);
				return NULL;
			}

			ShovelerComponent *textTextureRendererComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT_TEXTURE_RENDERER);
			assert(textTextureRendererComponent != NULL);
			ShovelerTextTextureRenderer *textTextureRenderer = shovelerComponentGetTextTextureRenderer(textTextureRendererComponent);
			assert(textTextureRenderer != NULL);

			if(!shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT)) {
				shovelerLogWarning("Failed to activate texture component of entity %lld: No text to render specified", component->entityId);
				return NULL;
			}

			const char *text = shovelerComponentGetConfigurationValueString(component, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT);
			assert(text != NULL);

			ShovelerRenderState *renderState = shovelerComponentGetViewRenderState(component);

			texture = shovelerTextTextureRendererRender(textTextureRenderer, text, renderState);
		} break;
		default:
			shovelerLogWarning("Failed to activate texture component of entity %lld: Unknown texture type %d", component->entityId, type);
			return NULL;
	}

	return texture;
}

static void deactivateTextureComponent(ShovelerComponent *component)
{
	ShovelerTexture *texture = (ShovelerTexture *) component->data;

	shovelerTextureFree(texture);
}
