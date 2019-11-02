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

ShovelerComponent *shovelerViewEntityAddTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTileSpriteAnimationComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTileSpriteAnimationType());
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
		shovelerLogWarning("Trying to update tile sprite animation of entity %lld which does not have a tile sprite animation, ignoring.", entity->id);
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
		shovelerLogWarning("Trying to remove tile sprite animation from entity %lld which does not have a tile sprite animation, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
}
