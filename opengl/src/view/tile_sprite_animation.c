#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/position.h"
#include "shoveler/view/tile_sprite.h"
#include "shoveler/view/tile_sprite_animation.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

static void *activateTileSpriteAnimationComponent(ShovelerComponent *component);
static void deactivateTileSpriteAnimationComponent(ShovelerComponent *component);
static void updateTileSpriteAnimationPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getTileSpriteAnimationPosition(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTileSpriteAnimationComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTileSpriteAnimationComponentTypeName, activateTileSpriteAnimationComponent, deactivateTileSpriteAnimationComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpriteAnimationPositionOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateTileSpriteAnimationPositionDependency);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpriteAnimationTileSpriteOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteAnimationPositionMappingXOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteAnimationPositionMappingYOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpriteAnimationPositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpriteAnimationTileSpriteOptionKey, configuration->tileSpriteEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteAnimationPositionMappingXOptionKey, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteAnimationPositionMappingYOptionKey, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey, configuration->moveAmountThreshold);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTileSpriteAnimation *shovelerViewEntityGetTileSpriteAnimation(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
	if(component == NULL) {
		return false;
	}

	return component->data;
}

bool shovelerViewEntityGetTileSpriteAnimationConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteAnimationConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteAnimationPositionOptionKey);
	outputConfiguration->tileSpriteEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteAnimationTileSpriteOptionKey);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteAnimationPositionMappingXOptionKey);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteAnimationPositionMappingYOptionKey);
	outputConfiguration->moveAmountThreshold = shovelerComponentGetConfigurationValueFloat(component, shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey);
	return true;
}

bool shovelerViewEntityUpdateTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tile sprite animation of entity %lld which does not have a tile sprite animation, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpriteAnimationPositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpriteAnimationTileSpriteOptionKey, configuration->tileSpriteEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteAnimationPositionMappingXOptionKey, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteAnimationPositionMappingYOptionKey, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey, configuration->moveAmountThreshold);
	return true;
}

bool shovelerViewEntityRemoveTileSpriteAnimation(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tile sprite animation from entity %lld which does not have a tile sprite animation, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
}

static void *activateTileSpriteAnimationComponent(ShovelerComponent *component)
{
	long long int tileSpriteEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteAnimationTileSpriteOptionKey);
	ShovelerViewEntity *tileSpriteEntity = shovelerViewGetEntity(component->entity->view, tileSpriteEntityId);
	assert(tileSpriteEntity != NULL);
	ShovelerCanvasTileSprite *tileSprite = shovelerViewEntityGetTileSprite(tileSpriteEntity);
	assert(tileSprite != NULL);

	if(tileSprite->tileset->columns < 4 || tileSprite->tileset->rows < 3) {
		shovelerLogWarning("Failed to activate tile sprite animation of entity %lld because the tileset of dependency tile sprite on entity %lld doesn't have enough columns and rows.", component->entity->entityId, tileSpriteEntityId);
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
	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteAnimationPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteAnimationPositionMappingXOptionKey);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteAnimationPositionMappingYOptionKey);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
