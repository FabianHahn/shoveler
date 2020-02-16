#include "shoveler/component/sprite.h"

#include <assert.h> // assert
#include <stdbool.h> // bool

#include "shoveler/canvas.h"
#include "shoveler/component/canvas.h"
#include "shoveler/component/position.h"
#include "shoveler/component/text_sprite.h"
#include "shoveler/component/tile_sprite.h"
#include "shoveler/component/tilemap_sprite.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/sprite.h"

const char *const shovelerComponentTypeIdSprite = "sprite";

static void *activateSpriteComponent(ShovelerComponent *component);
static void deactivateSpriteComponent(ShovelerComponent *component);
static void updateSpritePositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getSpritePosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateSpriteType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[9];
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION] = shovelerComponentTypeConfigurationOptionDependency("position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateSpritePositionDependency);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X] = shovelerComponentTypeConfigurationOption("position_mapping_x", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y] = shovelerComponentTypeConfigurationOption("position_mapping_y", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE] = shovelerComponentTypeConfigurationOption("size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS] = shovelerComponentTypeConfigurationOptionDependency("canvas", shovelerComponentTypeIdCanvas, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER] = shovelerComponentTypeConfigurationOption("layer", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_TEXT_SPRITE] = shovelerComponentTypeConfigurationOptionDependency("text_sprite", shovelerComponentTypeIdTextSprite, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE] = shovelerComponentTypeConfigurationOptionDependency("tile_sprite", shovelerComponentTypeIdTileSprite, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILEMAP_SPRITE] = shovelerComponentTypeConfigurationOptionDependency("tilemap_sprite", shovelerComponentTypeIdTilemapSprite, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdSprite, activateSpriteComponent, deactivateSpriteComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerSprite *shovelerComponentGetSprite(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdSprite);

	return component->data;
}

static void *activateSpriteComponent(ShovelerComponent *component)
{
	bool hasTextSprite = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TEXT_SPRITE);
	bool hasTileSprite = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE);
	bool hasTilemapSprite = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILEMAP_SPRITE);
	int numDependencies = (hasTextSprite ? 1 : 0) + (hasTileSprite ? 1 : 0) + (hasTilemapSprite ? 1 : 0);

	if(numDependencies != 1) {
		shovelerLogWarning("Failed to activate canvas component of entity %lld: Exactly one dependency option must be set, but found %d set options.", component->entityId, numDependencies);
		return NULL;
	}

	ShovelerSprite *sprite = NULL;
	if(hasTextSprite) {
		ShovelerComponent *textSpriteComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TEXT_SPRITE);
		assert(textSpriteComponent != NULL);
		sprite = shovelerComponentGetTextSprite(textSpriteComponent);
	}

	if(hasTileSprite) {
		ShovelerComponent *tileSpriteComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE);
		assert(tileSpriteComponent != NULL);
		sprite = shovelerComponentGetTileSprite(tileSpriteComponent);
	}

	if(hasTilemapSprite) {
		ShovelerComponent *tilemapSpriteComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILEMAP_SPRITE);
		assert(tilemapSpriteComponent != NULL);
		sprite = shovelerComponentGetTilemapSprite(tilemapSpriteComponent);
	}

	assert(sprite != NULL);

	shovelerSpriteUpdatePosition(sprite, getSpritePosition(component));
	sprite->size = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE);

	ShovelerComponent *canvasComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS);
	assert(canvasComponent != NULL);
	ShovelerCanvas *canvas = shovelerComponentGetCanvas(canvasComponent);

	int layerId = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER);
	shovelerCanvasAddSprite(canvas, layerId, sprite);

	return sprite;
}

static void deactivateSpriteComponent(ShovelerComponent *component)
{
	ShovelerSprite *sprite = (ShovelerSprite *) component->data;
	assert(sprite != NULL);

	ShovelerComponent *canvasComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS);
	assert(canvasComponent != NULL);
	ShovelerCanvas *canvas = shovelerComponentGetCanvas(canvasComponent);

	int layerId = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER);
	shovelerCanvasRemoveSprite(canvas, layerId, sprite);
}

static void updateSpritePositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerSprite *sprite = (ShovelerSprite *) component->data;
	assert(sprite != NULL);

	shovelerSpriteUpdatePosition(sprite, getSpritePosition(component));
}

static ShovelerVector2 getSpritePosition(ShovelerComponent *component)
{
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = (ShovelerCoordinateMapping) shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X);
	ShovelerCoordinateMapping positionMappingY = (ShovelerCoordinateMapping) shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
