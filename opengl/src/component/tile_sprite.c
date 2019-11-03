#include "shoveler/component/tile_sprite.h"

#include <stdlib.h> // malloc free

#include "shoveler/component/position.h"
#include "shoveler/component/tileset.h"
#include "shoveler/canvas.h"
#include "shoveler/component.h"

const char *const shovelerComponentTypeIdTileSprite = "tile_sprite";

static void *activateTileSpriteComponent(ShovelerComponent *component);
static void deactivateTileSpriteComponent(ShovelerComponent *component);
static void updateTileSpritePositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getTileSpritePosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTileSpriteType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[7];
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION] = shovelerComponentTypeConfigurationOptionDependency("position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateTileSpritePositionDependency);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET] = shovelerComponentTypeConfigurationOptionDependency("tileset", shovelerComponentTypeIdTileset, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_COLUMN] = shovelerComponentTypeConfigurationOption("tileset_column", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_ROW] = shovelerComponentTypeConfigurationOption("tileset_row", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_X] = shovelerComponentTypeConfigurationOption("position_mapping_x", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_Y] = shovelerComponentTypeConfigurationOption("position_mapping_y", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_OPTION_SIZE] = shovelerComponentTypeConfigurationOption("size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTileSprite, activateTileSpriteComponent, deactivateTileSpriteComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerCanvasTileSprite *shovelerComponentGetTileSprite(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTileSprite);

	return component->data;
}

static void *activateTileSpriteComponent(ShovelerComponent *component)
{
	ShovelerComponent *tilesetComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET);
	assert(tilesetComponent != NULL);
	ShovelerTileset *tileset = shovelerComponentGetTileset(tilesetComponent);
	assert(tileset != NULL);

	int tilesetColumn = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_COLUMN);
	int tilesetRow = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_ROW);
	ShovelerVector2 position = getTileSpritePosition(component);
	ShovelerVector2 size = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_SIZE);

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

static void updateTileSpritePositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerCanvasTileSprite *tileSprite = (ShovelerCanvasTileSprite *) component->data;
	assert(tileSprite != NULL);

	tileSprite->position = getTileSpritePosition(component);
}

static ShovelerVector2 getTileSpritePosition(ShovelerComponent *component)
{
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_X);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_Y);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
