#include "shoveler/view/sprite.h"

#include "shoveler/view/position.h"
#include "shoveler/view/tile_sprite.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddSprite(ShovelerViewEntity *entity, const ShovelerViewSpriteConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdSprite)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateSpriteType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS, configuration->canvasEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER, configuration->layer);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE, configuration->tileSpriteEntityId);

	shovelerComponentActivate(component);
	return component;
}

ShovelerSprite *shovelerViewEntityGetSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSprite);
	if(component == NULL) {
		return false;
	}

	return component->data;
}

bool shovelerViewEntityGetSpriteConfiguration(ShovelerViewEntity *entity, ShovelerViewSpriteConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSprite);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y);
	outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS);
	outputConfiguration->layer = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE);
	outputConfiguration->tileSpriteEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE);
	return true;
}

bool shovelerViewEntityUpdateSprite(ShovelerViewEntity *entity, const ShovelerViewSpriteConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSprite);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas tile sprite of entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_CANVAS, configuration->canvasEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_LAYER, configuration->layer);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_SIZE, configuration->size);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_SPRITE_OPTION_ID_TILE_SPRITE, configuration->positionEntityId);
	return true;
}

bool shovelerViewEntityRemoveSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSprite);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas tile sprite from entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdSprite);
}
