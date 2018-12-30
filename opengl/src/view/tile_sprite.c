#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/position.h"
#include "shoveler/view/tile_sprite.h"
#include "shoveler/view/tileset.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTileSpriteConfiguration configuration;
	ShovelerCanvasTileSprite *tileSprite;
	ShovelerViewComponentCallback *positionCallback;
} ComponentData;

static void assignConfiguration(ShovelerViewTileSpriteConfiguration *destination, const ShovelerViewTileSpriteConfiguration *source);
static void clearConfiguration(ShovelerViewTileSpriteConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *componentDataPointer);
static void setTileSpritePosition(ComponentData *componentData, const ShovelerViewPosition *position);
static double mapCoordinate(const ShovelerViewPosition *position, ShovelerViewTileSpriteCoordinateMappingConfiguration mapping);

bool shovelerViewEntityAddTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add canvas tile sprite to entity %lld which already has canvas tile sprite, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.tilesetEntityId = 0;
	componentData->configuration.tilesetColumn = 0;
	componentData->configuration.tilesetRow = 0;
	componentData->configuration.positionMappingX = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_X;
	componentData->configuration.positionMappingY = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Y;
	componentData->configuration.size = shovelerVector2(0.0f, 0.0f);
	componentData->tileSprite = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewTileSpriteComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);
	shovelerViewComponentAddDependency(component, componentData->configuration.tilesetEntityId, shovelerViewTilesetComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerCanvasTileSprite *shovelerViewEntityGetTileSprite(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return componentData->tileSprite;
}

const ShovelerViewTileSpriteConfiguration *shovelerViewEntityGetTileSpriteConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTileSprite(ShovelerViewEntity *entity, const ShovelerViewTileSpriteConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas tile sprite of entity %lld which does not have a canvas tile sprite, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tilesetEntityId, shovelerViewTileSpriteComponentName)) {
		return false;
	}

	assignConfiguration(&componentData->configuration, configuration);

	shovelerViewComponentAddDependency(component, componentData->configuration.tilesetEntityId, shovelerViewTileSpriteComponentName);

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTileSprite(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas tile sprite from entity %lld which does not have a canvas tile sprite, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTileSpriteComponentName);
}

static void assignConfiguration(ShovelerViewTileSpriteConfiguration *destination, const ShovelerViewTileSpriteConfiguration *source)
{
	clearConfiguration(destination);

	destination->tilesetEntityId = source->tilesetEntityId;
	destination->tilesetColumn = source->tilesetColumn;
	destination->tilesetRow = source->tilesetRow;
	destination->positionMappingX = source->positionMappingX;
	destination->positionMappingY = source->positionMappingY;
	destination->size = source->size;
}

static void clearConfiguration(ShovelerViewTileSpriteConfiguration *configuration)
{
	configuration->tilesetEntityId = 0;
	configuration->tilesetColumn = 0;
	configuration->tilesetRow = 0;
	configuration->positionMappingX = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_X;
	configuration->positionMappingY = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Y;
	configuration->size = shovelerVector2(0.0f, 0.0f);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	ShovelerViewEntity *tilesetEntityId = shovelerViewGetEntity(component->entity->view, componentData->configuration.tilesetEntityId);
	assert(tilesetEntityId != NULL);
	ShovelerTileset *tileset = shovelerViewEntityGetTileset(tilesetEntityId);
	assert(tileset != NULL);

	componentData->tileSprite = malloc(sizeof(ShovelerCanvasTileSprite));
	componentData->tileSprite->tileset = tileset;
	componentData->tileSprite->tilesetColumn = componentData->configuration.tilesetColumn;
	componentData->tileSprite->tilesetRow = componentData->configuration.tilesetRow;
	componentData->tileSprite->size = componentData->configuration.size;

	setTileSpritePosition(componentData, shovelerViewEntityGetPosition(component->entity));

	componentData->positionCallback = shovelerViewEntityAddCallback(component->entity, shovelerViewPositionComponentName, &positionCallback, componentData);

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	free(componentData->tileSprite);
	componentData->tileSprite = NULL;

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, componentData->positionCallback);
	componentData->positionCallback = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	free(componentData->tileSprite);

	clearConfiguration(&componentData->configuration);
	free(componentData);
}

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	setTileSpritePosition(componentData, shovelerViewEntityGetPosition(positionComponent->entity));
}

static void setTileSpritePosition(ComponentData *componentData, const ShovelerViewPosition *position)
{
	componentData->tileSprite->position.values[0] = (float) mapCoordinate(position, componentData->configuration.positionMappingX);
	componentData->tileSprite->position.values[1] = (float) mapCoordinate(position, componentData->configuration.positionMappingY);
}

static double mapCoordinate(const ShovelerViewPosition *position, ShovelerViewTileSpriteCoordinateMappingConfiguration mapping)
{
	switch(mapping) {
		case SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_X:
			return position->x;
		case SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_NEGATIVE_X:
			return -position->x;
		case SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Y:
			return position->y;
		case SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_NEGATIVE_Y:
			return -position->y;
		case SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Z:
			return position->z;
		case SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_NEGATIVE_Z:
			return -position->z;
		default:
			shovelerLogError("Unknown coordinate mapping configuration: %d", mapping);
			return 0.0;
	}
}
