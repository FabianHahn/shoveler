#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/position.h"
#include "shoveler/view/tile_sprite.h"
#include "shoveler/view/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

static void *activateTileSpriteComponent(ShovelerComponent *component);
static void deactivateTileSpriteComponent(ShovelerComponent *component);
static void updateTileSpritePositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static ShovelerVector2 getTileSpritePosition(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewTileSpriteComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTileSpriteComponentTypeName, activateTileSpriteComponent, deactivateTileSpriteComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpritePositionOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateTileSpritePositionDependency);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTileSpriteTilesetOptionKey, shovelerViewTilesetComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteTilesetColumnOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteTilesetRowOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpritePositionMappingXOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpritePositionMappingYOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewTileSpriteSizeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
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
		shovelerLogWarning("Trying to update canvas tile sprite of entity %lld which does not have a canvas tile sprite, ignoring.", entity->entityId);
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
		shovelerLogWarning("Trying to remove canvas tile sprite from entity %lld which does not have a canvas tile sprite, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTileSpriteComponentTypeName);
}

static void *activateTileSpriteComponent(ShovelerComponent *component)
{
	long long int tilesetEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpriteTilesetOptionKey);
	ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, tilesetEntityId);
	assert(tilesetEntity != NULL);
	ShovelerTileset *tileset = shovelerViewEntityGetTileset(tilesetEntity);
	assert(tileset != NULL);

	int tilesetColumn = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteTilesetColumnOptionKey);
	int tilesetRow = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpriteTilesetRowOptionKey);
	ShovelerVector2 position = getTileSpritePosition(component);
	ShovelerVector2 size = shovelerComponentGetConfigurationValueVector2(component, shovelerViewTileSpriteSizeOptionKey);

	ShovelerCanvasTileSprite *tileSprite = malloc(sizeof(ShovelerCanvasTileSprite));
	tileSprite->tileset = tileset;
	tileSprite->tilesetColumn = tilesetColumn;
	tileSprite->tilesetRow = tilesetRow;
	tileSprite->position = position;
	tileSprite->size = size;
	return tileSprite;
}

static void deactivateTileSpriteComponent(ShovelerComponent *component)
{
	ShovelerCanvasTileSprite *tileSprite = (ShovelerCanvasTileSprite *) component->data;
	assert(tileSprite != NULL);

	free(tileSprite);
}

static void updateTileSpritePositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerCanvasTileSprite *tileSprite = (ShovelerCanvasTileSprite *) component->data;
	assert(tileSprite != NULL);

	tileSprite->position = getTileSpritePosition(component);
}

static ShovelerVector2 getTileSpritePosition(ShovelerComponent *component)
{
	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewTileSpritePositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	ShovelerCoordinateMapping positionMappingX = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpritePositionMappingXOptionKey);
	ShovelerCoordinateMapping positionMappingY = shovelerComponentGetConfigurationValueInt(component, shovelerViewTileSpritePositionMappingYOptionKey);

	return shovelerVector2(
		shovelerCoordinateMap(*positionCoordinates, positionMappingX),
		shovelerCoordinateMap(*positionCoordinates, positionMappingY));
}
