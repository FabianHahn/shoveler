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
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE, configuration->tileSpriteEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD, configuration->moveAmountThreshold);

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

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION);
	outputConfiguration->tileSpriteEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y);
	outputConfiguration->moveAmountThreshold = shovelerComponentGetConfigurationValueFloat(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD);
	return true;
}

bool shovelerViewEntityUpdateTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tile sprite animation of entity %lld which does not have a tile sprite animation, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_TILE_SPRITE, configuration->tileSpriteEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionFloat(component, SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_OPTION_MOVE_AMOUNT_THRESHOLD, configuration->moveAmountThreshold);
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
