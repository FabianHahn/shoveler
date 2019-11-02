#include "shoveler/component/tile_sprite_animation.h"

#include <string.h> // strcmp

#include "shoveler/component/position.h"
#include "shoveler/component/tile_sprite.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/tile_sprite_animation.h"

static void *activateTileSpriteAnimationComponent(ShovelerComponent *component);
static void deactivateTileSpriteAnimationComponent(ShovelerComponent *component);
static void updateTileSpriteAnimationPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTileSpriteAnimationType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTileSpriteAnimationComponentTypeName, activateTileSpriteAnimationComponent, deactivateTileSpriteAnimationComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpriteAnimationPositionOptionKey, shovelerComponentTypeNamePosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateTileSpriteAnimationPositionDependency);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpriteAnimationTileSpriteOptionKey, shovelerViewTileSpriteComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteAnimationPositionMappingXOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteAnimationPositionMappingYOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerTileSpriteAnimation *shovelerComponentGetTileSpriteAnimation(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTileSpriteAnimationComponentTypeName) == 0);

	return component->data;
}

static void *activateTileSpriteAnimationComponent(ShovelerComponent *component)
{
	ShovelerComponent *tileSpriteComponent = shovelerComponentGetDependency(component, shovelerViewTileSpriteAnimationTileSpriteOptionKey);
	assert(tileSpriteComponent != NULL);
	ShovelerCanvasTileSprite *tileSprite = shovelerComponentGetTileSprite(tileSpriteComponent);
	assert(tileSprite != NULL);

	if(tileSprite->tileset->columns < 4 || tileSprite->tileset->rows < 3) {
		shovelerLogWarning("Failed to activate tile sprite animation of entity %lld because the tileset of dependency tile sprite on entity %lld doesn't have enough columns and rows.", component->entityId, tileSpriteComponent->entityId);
		return false;
	}

	float moveAmountThreshold = shovelerComponentGetConfigurationValueFloat(component, shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey);
	ShovelerTileSpriteAnimation *animation = shovelerTileSpriteAnimationCreate(tileSprite, getTileSpriteAnimationPosition(component), moveAmountThreshold);
	return animation;
}

static void deactivateTileSpriteAnimationComponent(ShovelerComponent *component)
{
	ShovelerTileSpriteAnimation *animation = (ShovelerTileSpriteAnimation *) component->data;

	shovelerTileSpriteAnimationFree(animation);
}

static void updateTileSpriteAnimationPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerTileSpriteAnimation *animation = (ShovelerTileSpriteAnimation *) component->data;
	assert(animation != NULL);

	shovelerTileSpriteAnimationUpdate(animation, getTileSpriteAnimationPosition(component));
}

static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent *component)
{
	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewTileSpriteAnimationPositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteAnimationPositionMappingXOptionKey);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteAnimationPositionMappingYOptionKey);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
