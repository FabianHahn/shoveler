#include "shoveler/component/tile_sprite_animation.h"

#include "shoveler/component/position.h"
#include "shoveler/component/tile_sprite.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/tile_sprite_animation.h"
#include "shoveler/tileset.h"

const char *const shovelerComponentTypeIdTileSpriteAnimation = "tile_sprite_animation";

static void *activateTileSpriteAnimationComponent(ShovelerComponent *component);
static void deactivateTileSpriteAnimationComponent(ShovelerComponent *component);
static void liveUpdateTileSpriteAnimationPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTileSpriteAnimationType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[5];
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION] = shovelerComponentTypeConfigurationOptionDependency("position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, liveUpdateTileSpriteAnimationPositionDependency);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE] = shovelerComponentTypeConfigurationOptionDependency("tile_sprite", shovelerComponentTypeIdTileSprite, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X] = shovelerComponentTypeConfigurationOption("position_mapping_x", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y] = shovelerComponentTypeConfigurationOption("position_mapping_y", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD] = shovelerComponentTypeConfigurationOption("move_amount_threshold", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTileSpriteAnimation, activateTileSpriteAnimationComponent, /* update */ NULL, deactivateTileSpriteAnimationComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTileSpriteAnimation *shovelerComponentGetTileSpriteAnimation(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTileSpriteAnimation);

	return component->data;
}

static void *activateTileSpriteAnimationComponent(ShovelerComponent *component)
{
	ShovelerComponent *tileSpriteComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE);
	assert(tileSpriteComponent != NULL);
	ShovelerSprite *tileSprite = shovelerComponentGetTileSprite(tileSpriteComponent);
	assert(tileSprite != NULL);
	ShovelerTileset *tileset = shovelerSpriteTileGetTileset(tileSprite);
	assert(tileset != NULL);

	if(tileset->columns < 4 || tileset->rows < 3) {
		shovelerLogWarning("Failed to activate tile sprite animation of entity %lld because the tileset of dependency tile sprite on entity %lld doesn't have enough columns and rows.", component->entityId, tileSpriteComponent->entityId);
		return false;
	}

	float moveAmountThreshold = shovelerComponentGetConfigurationValueFloat(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD);
	ShovelerTileSpriteAnimation *animation = shovelerTileSpriteAnimationCreate(tileSprite, getTileSpriteAnimationPosition(component), moveAmountThreshold);
	return animation;
}

static void deactivateTileSpriteAnimationComponent(ShovelerComponent *component)
{
	ShovelerTileSpriteAnimation *animation = (ShovelerTileSpriteAnimation *) component->data;

	shovelerTileSpriteAnimationFree(animation);
}

static void liveUpdateTileSpriteAnimationPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerTileSpriteAnimation *animation = (ShovelerTileSpriteAnimation *) component->data;
	assert(animation != NULL);

	shovelerTileSpriteAnimationUpdate(animation, getTileSpriteAnimationPosition(component));
}

static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent *component)
{
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
