#include "shoveler/view/tile_sprite.h"

#include "shoveler/view/position.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdTileSprite)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTileSpriteType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdTileSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_MATERIAL, configuration->materialEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET, configuration->tilesetEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_COLUMN, configuration->tilesetColumn);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_ROW, configuration->tilesetRow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_SIZE, configuration->size);

	shovelerComponentActivate(component);
	return component;
}

ShovelerSprite *shovelerViewEntityGetTileSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSprite);
	if(component == NULL) {
		return false;
	}

	return component->data;
}

bool shovelerViewEntityGetTileSpriteConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSprite);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION);
	outputConfiguration->materialEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_MATERIAL);
	outputConfiguration->tilesetEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET);
	outputConfiguration->tilesetColumn = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_COLUMN);
	outputConfiguration->tilesetRow = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_ROW);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_X);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_Y);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_SIZE);
	return true;
}

bool shovelerViewEntityUpdateTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSprite);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas tile sprite of entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_MATERIAL, configuration->materialEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET, configuration->tilesetEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_COLUMN, configuration->tilesetColumn);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_TILESET_ROW, configuration->tilesetRow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_X, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_POSITION_MAPPING_Y, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_TILE_SPRITE_OPTION_SIZE, configuration->size);
	return true;
}

bool shovelerViewEntityRemoveTileSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSprite);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas tile sprite from entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdTileSprite);
}
