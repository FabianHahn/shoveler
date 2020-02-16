#include "shoveler/component/tilemap_sprite.h"

#include "shoveler/component/material.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component.h"
#include "shoveler/sprite/tilemap.h"

const char *const shovelerComponentTypeIdTilemapSprite = "tilemap_sprite";

static void *activateTilemapSpriteComponent(ShovelerComponent *component);
static void deactivateTilemapSpriteComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilemapSpriteType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[2];
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_MATERIAL] = shovelerComponentTypeConfigurationOptionDependency("material", shovelerComponentTypeIdMaterial, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_TILEMAP] = shovelerComponentTypeConfigurationOptionDependency("tilemap", shovelerComponentTypeIdTilemap, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTilemapSprite, activateTilemapSpriteComponent, deactivateTilemapSpriteComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerSprite *shovelerComponentGetTilemapSprite(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTilemapSprite);

	return component->data;
}

static void *activateTilemapSpriteComponent(ShovelerComponent *component)
{
	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_MATERIAL);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	ShovelerComponent *tilemapComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_SPRITE_OPTION_ID_TILEMAP);
	assert(tilemapComponent != NULL);
	ShovelerTilemap *tilemap = shovelerComponentGetTilemap(tilemapComponent);
	assert(tilemap != NULL);

	ShovelerSprite *tileSprite = shovelerSpriteTilemapCreate(material, tilemap);
	return tileSprite;
}

static void deactivateTilemapSpriteComponent(ShovelerComponent *component)
{
	ShovelerSprite *tileSprite = (ShovelerSprite *) component->data;
	assert(tileSprite != NULL);

	shovelerSpriteFree(tileSprite);
}
