#include "shoveler/component/tile_sprite.h"

#include <stdlib.h> // malloc free
#include <string.h> // strcmp

#include "shoveler/component/position.h"
#include "shoveler/component/tileset.h"
#include "shoveler/canvas.h"
#include "shoveler/component.h"

static void *activateTileSpriteComponent(ShovelerComponent *component);
static void deactivateTileSpriteComponent(ShovelerComponent *component);
static void updateTileSpritePositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getTileSpritePosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTileSpriteType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTileSpriteComponentTypeName, activateTileSpriteComponent, deactivateTileSpriteComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpritePositionOptionKey, shovelerComponentTypeNamePosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateTileSpritePositionDependency);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpriteTilesetOptionKey, shovelerViewTilesetComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteTilesetColumnOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteTilesetRowOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpritePositionMappingXOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpritePositionMappingYOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteSizeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);
}

ShovelerCanvasTileSprite *shovelerComponentGetTileSprite(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTileSpriteComponentTypeName) == 0);

	return component->data;
}

static void *activateTileSpriteComponent(ShovelerComponent *component)
{
	ShovelerComponent *tilesetComponent = shovelerComponentGetDependency(component, shovelerViewTileSpriteTilesetOptionKey);
	assert(tilesetComponent != NULL);
	ShovelerTileset *tileset = shovelerComponentGetTileset(tilesetComponent);
	assert(tileset != NULL);

	int tilesetColumn = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteTilesetColumnOptionKey);
	int tilesetRow = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteTilesetRowOptionKey);
	ShovelerVector2 position = getTileSpritePosition(component);
	ShovelerVector2 size = shovelerComponentGetConfigurationValueVector2(component, shovelerViewTileSpriteSizeOptionKey);

	ShovelerCanvasTileSprite *tileSprite = malloc(sizeof(ShovelerCanvasTileSprite));
	tileSprite->tileset = tileset;
	tileSprite->tilesetColumn = tilesetColumn;
	tileSprite->tilesetRow = tilesetRow;
	tileSprite->position = position;
	tileSprite->size = size;
	return tileSprite;
}

static void deactivateTileSpriteComponent(ShovelerComponent *component)
{
	ShovelerCanvasTileSprite *tileSprite = (ShovelerCanvasTileSprite *) component->data;
	assert(tileSprite != NULL);

	free(tileSprite);
}

static void updateTileSpritePositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerCanvasTileSprite *tileSprite = (ShovelerCanvasTileSprite *) component->data;
	assert(tileSprite != NULL);

	tileSprite->position = getTileSpritePosition(component);
}

static ShovelerVector2 getTileSpritePosition(ShovelerComponent *component)
{
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewTileSpritePositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpritePositionMappingXOptionKey);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpritePositionMappingYOptionKey);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
