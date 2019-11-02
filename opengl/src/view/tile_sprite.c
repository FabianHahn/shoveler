#include "shoveler/view/tile_sprite.h"

#include "shoveler/view/position.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameTileSprite)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateTileSpriteType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameTileSprite);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteOptionKeyTileset, configuration->tilesetEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyTilesetColumn, configuration->tilesetColumn);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyTilesetRow, configuration->tilesetRow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyPositionMappingX, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyPositionMappingY, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentTileSpriteOptionKeySize, configuration->size);

	shovelerComponentActivate(component);
	return component;
}

ShovelerCanvasTileSprite *shovelerViewEntityGetTileSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileSprite);
	if(component == NULL) {
		return false;
	}

	return component->data;
}

bool shovelerViewEntityGetTileSpriteConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileSprite);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTileSpriteOptionKeyPosition);
	outputConfiguration->tilesetEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentTileSpriteOptionKeyTileset);
	outputConfiguration->tilesetColumn = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTileSpriteOptionKeyTilesetColumn);
	outputConfiguration->tilesetRow = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTileSpriteOptionKeyTilesetRow);
	outputConfiguration->positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTileSpriteOptionKeyPositionMappingX);
	outputConfiguration->positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerComponentTileSpriteOptionKeyPositionMappingY);
	outputConfiguration->size = shovelerComponentGetConfigurationValueVector2(component, shovelerComponentTileSpriteOptionKeySize);
	return true;
}

bool shovelerViewEntityUpdateTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileSprite);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas tile sprite of entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentTileSpriteOptionKeyTileset, configuration->tilesetEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyTilesetColumn, configuration->tilesetColumn);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyTilesetRow, configuration->tilesetRow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyPositionMappingX, configuration->positionMappingX);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentTileSpriteOptionKeyPositionMappingY, configuration->positionMappingY);
	shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentTileSpriteOptionKeySize, configuration->size);
	return true;
}

bool shovelerViewEntityRemoveTileSprite(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameTileSprite);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas tile sprite from entity %lld which does not have a canvas tile sprite, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameTileSprite);
}
