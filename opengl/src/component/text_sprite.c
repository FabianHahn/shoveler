#include "shoveler/component/text_sprite.h"

#include "shoveler/component/material.h"
#include "shoveler/component/position.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/sprite/text.h"

const char *const shovelerComponentTypeIdTextSprite = "text_sprite";

static void *activateTextSpriteComponent(ShovelerComponent *component);
static void deactivateTextSpriteComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[5];
	configurationOptions[SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_MATERIAL] = shovelerComponentTypeConfigurationOptionDependency("material", shovelerComponentTypeIdMaterial, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_FONT_ATLAS] = shovelerComponentTypeConfigurationOptionDependency("font_atlas", /* TODO */ shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_FONT_SIZE] = shovelerComponentTypeConfigurationOption("font_size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_CONTENT] = shovelerComponentTypeConfigurationOption("content", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_COLOR] = shovelerComponentTypeConfigurationOption("color", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTextSprite, activateTextSpriteComponent, /* update */ NULL, deactivateTextSpriteComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerSprite *shovelerComponentGetTextSprite(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTextSprite);

	return component->data;
}

static void *activateTextSpriteComponent(ShovelerComponent *component)
{
	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXT_SPRITE_OPTION_ID_MATERIAL);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	// TODO: get font atlast texture

	// TODO: create text sprite
	shovelerLogWarning("Failed to activate text component on entity %lld - not implemented yet.", component->entityId);
	return NULL;
}

static void deactivateTextSpriteComponent(ShovelerComponent *component)
{
	ShovelerSprite *textSprite = (ShovelerSprite *) component->data;
	assert(textSprite != NULL);

	shovelerSpriteFree(textSprite);
}
