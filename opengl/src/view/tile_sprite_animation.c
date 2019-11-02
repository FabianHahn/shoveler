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
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTileSpriteAnimation)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTileSpriteAnimationType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteAnimationOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteAnimationOptionKeyTileSprite, configuration->tileSpriteEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteAnimationOptionKeyPositionMappingX, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteAnimationOptionKeyPositionMappingY, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerComponentTileSpriteAnimationOptionKeyMoveAmountThreshold, configuration->moveAmountThreshold);

	shovelerComponentActivate(component);
	return component;
}

ShovelerTileSpriteAnimation *shovelerViewEntityGetTileSpriteAnimation(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
	if(component == NULL) {
		return false;
	}

	return component->data;
}

bool shovelerViewEntityGetTileSpriteAnimationConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteAnimationConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTileSpriteAnimationOptionKeyPosition);
	outputConfiguration->tileSpriteEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTileSpriteAnimationOptionKeyTileSprite);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTileSpriteAnimationOptionKeyPositionMappingX);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTileSpriteAnimationOptionKeyPositionMappingY);
	outputConfiguration->moveAmountThreshold = shovelerComponentGetConfigurationValueFloat(component, shovelerComponentTileSpriteAnimationOptionKeyMoveAmountThreshold);
	return true;
}

bool shovelerViewEntityUpdateTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tile sprite animation of entity %lld which does not have a tile sprite animation, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteAnimationOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteAnimationOptionKeyTileSprite, configuration->tileSpriteEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteAnimationOptionKeyPositionMappingX, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteAnimationOptionKeyPositionMappingY, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, shovelerComponentTileSpriteAnimationOptionKeyMoveAmountThreshold, configuration->moveAmountThreshold);
	return true;
}

bool shovelerViewEntityRemoveTileSpriteAnimation(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tile sprite animation from entity %lld which does not have a tile sprite animation, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
}
