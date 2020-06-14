#include "shoveler/component/texture_sprite.h"

#include "shoveler/component/material.h"
#include "shoveler/component/texture.h"
#include "shoveler/component.h"
#include "shoveler/sprite/texture.h"

const char *const shovelerComponentTypeIdTextureSprite = "texture_sprite";

static void *activateTextureSpriteComponent(ShovelerComponent *component);
static void deactivateTextureSpriteComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTextureSpriteType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[2];
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_SPRITE_OPTION_ID_MATERIAL] = shovelerComponentTypeConfigurationOptionDependency("material", shovelerComponentTypeIdMaterial, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TEXTURE_SPRITE_OPTION_ID_TEXTURE] = shovelerComponentTypeConfigurationOptionDependency("texture", shovelerComponentTypeIdTexture, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTextureSprite, activateTextureSpriteComponent, /* update */ NULL, deactivateTextureSpriteComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerSprite *shovelerComponentGetTextureSprite(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTextureSprite);

	return component->data;
}

static void *activateTextureSpriteComponent(ShovelerComponent *component)
{
	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_SPRITE_OPTION_ID_TEXTURE);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	ShovelerComponent *textureComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_SPRITE_OPTION_ID_TEXTURE);
	assert(textureComponent != NULL);
	ShovelerTexture *texture = shovelerComponentGetTexture(textureComponent);
	assert(texture != NULL);

	ShovelerSprite *textureSprite = shovelerSpriteTextureCreate(material, texture);
	return textureSprite;
}

static void deactivateTextureSpriteComponent(ShovelerComponent *component)
{
	ShovelerSprite *textureSprite = (ShovelerSprite *) component->data;
	assert(textureSprite != NULL);

	shovelerSpriteFree(textureSprite);
}
