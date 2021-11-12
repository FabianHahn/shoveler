#include "shoveler/component/tile_sprite.h"

#include "shoveler/component/material.h"
#include "shoveler/component/tileset.h"
#include "shoveler/component.h"
#include "shoveler/sprite/tile.h"

const char *const shovelerComponentTypeIdTileSprite = "tile_sprite";

static void *activateTileSpriteComponent(ShovelerComponent *component);
static void deactivateTileSpriteComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTileSpriteType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[4];
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_MATERIAL] = shovelerComponentTypeConfigurationOptionDependency("material", shovelerComponentTypeIdMaterial, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET] = shovelerComponentTypeConfigurationOptionDependency("tileset", shovelerComponentTypeIdTileset, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_COLUMN] = shovelerComponentTypeConfigurationOption("tileset_column", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_ROW] = shovelerComponentTypeConfigurationOption("tileset_row", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTileSprite, activateTileSpriteComponent, /* update */ NULL, deactivateTileSpriteComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerSprite *shovelerComponentGetTileSprite(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTileSprite);

	return component->data;
}

static void *activateTileSpriteComponent(ShovelerComponent *component)
{
	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_MATERIAL);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	ShovelerComponent *tilesetComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET);
	assert(tilesetComponent != NULL);
	ShovelerTileset *tileset = shovelerComponentGetTileset(tilesetComponent);
	assert(tileset != NULL);

	int tilesetColumn = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_COLUMN);
	int tilesetRow = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_ID_TILESET_ROW);

	ShovelerSprite *tileSprite = shovelerSpriteTileCreate(material, tileset, tilesetColumn, tilesetRow);
	return tileSprite;
}

static void deactivateTileSpriteComponent(ShovelerComponent *component)
{
	ShovelerSprite *tileSprite = (ShovelerSprite *) component->data;
	assert(tileSprite != NULL);

	shovelerSpriteFree(tileSprite);
}
