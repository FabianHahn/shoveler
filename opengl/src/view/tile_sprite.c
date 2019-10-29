#include "shoveler/view/tile_sprite.h"

#include "shoveler/view/position.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTileSpriteComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTileSpriteType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewTileSpriteComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpritePositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpriteTilesetOptionKey, configuration->tilesetEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteTilesetColumnOptionKey, configuration->tilesetColumn);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteTilesetRowOptionKey, configuration->tilesetRow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpritePositionMappingXOptionKey, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpritePositionMappingYOptionKey, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewTileSpriteSizeOptionKey, configuration->size);

	shovelerComponentActivate(component);
	return component;
}

ShovelerCanvasTileSprite *shovelerViewEntityGetTileSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentTypeName);
	if(component == NULL) {
		return false;
	}

	return component->data;
}

bool shovelerViewEntityGetTileSpriteConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpritePositionOptionKey);
	outputConfiguration->tilesetEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteTilesetOptionKey);
	outputConfiguration->tilesetColumn = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteTilesetColumnOptionKey);
	outputConfiguration->tilesetRow = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteTilesetRowOptionKey);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpritePositionMappingXOptionKey);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpritePositionMappingYOptionKey);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, shovelerViewTileSpriteSizeOptionKey);
	return true;
}

bool shovelerViewEntityUpdateTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas tile sprite of entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpritePositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewTileSpriteTilesetOptionKey, configuration->tilesetEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteTilesetColumnOptionKey, configuration->tilesetColumn);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpriteTilesetRowOptionKey, configuration->tilesetRow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpritePositionMappingXOptionKey, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewTileSpritePositionMappingYOptionKey, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewTileSpriteSizeOptionKey, configuration->size);
	return true;
}

bool shovelerViewEntityRemoveTileSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas tile sprite from entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTileSpriteComponentTypeName);
}
