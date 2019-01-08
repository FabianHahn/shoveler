#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memmove

#include "shoveler/view/position.h"
#include "shoveler/view/tile_sprite.h"
#include "shoveler/view/tile_sprite_animation.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTileSpriteAnimationConfiguration configuration;
	ShovelerTileSpriteAnimation *animation;
	ShovelerViewTileSpriteCoordinateMappingConfiguration positionMappingX;
	ShovelerViewTileSpriteCoordinateMappingConfiguration positionMappingY;
	float previousPositionX;
	float previousPositionY;
	ShovelerViewComponentCallback *positionCallback;
} ComponentData;

static void assignConfiguration(ShovelerViewTileSpriteAnimationConfiguration *destination, const ShovelerViewTileSpriteAnimationConfiguration *source);
static void clearConfiguration(ShovelerViewTileSpriteAnimationConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *componentDataPointer);
static void updatePosition(ComponentData *componentData, const ShovelerViewPosition *position);

bool shovelerViewEntityAddTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tile sprite animation to entity %lld which already has tile sprite animation, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.tileSpriteEntityId = 0;
	componentData->configuration.moveAmountThreshold = 0.0f;
	componentData->animation = NULL;
	componentData->positionMappingX = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_X;
	componentData->positionMappingY = SHOVELER_VIEW_TILE_SPRITE_COORDINATE_MAPPING_POSITIVE_Y;
	componentData->previousPositionX = 0.0f;
	componentData->previousPositionY = 0.0f;
	componentData->positionCallback = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewTileSpriteAnimationComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);
	shovelerViewComponentAddDependency(component, componentData->configuration.tileSpriteEntityId, shovelerViewTileSpriteComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerTileSpriteAnimation *shovelerViewEntityGetTileSpriteAnimation(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return componentData->animation;
}

const ShovelerViewTileSpriteAnimationConfiguration *shovelerViewEntityGetTileSpriteAnimationConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tile sprite animation of entity %lld which does not have a tile sprite animation, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tileSpriteEntityId, shovelerViewTileSpriteAnimationComponentName)) {
		return false;
	}

	assignConfiguration(&componentData->configuration, configuration);

	shovelerViewComponentAddDependency(component, componentData->configuration.tileSpriteEntityId, shovelerViewTileSpriteAnimationComponentName);

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTileSpriteAnimation(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tile sprite animation from entity %lld which does not have a tile sprite animation, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTileSpriteAnimationComponentName);
}

static void assignConfiguration(ShovelerViewTileSpriteAnimationConfiguration *destination, const ShovelerViewTileSpriteAnimationConfiguration *source)
{
	clearConfiguration(destination);

	destination->tileSpriteEntityId = source->tileSpriteEntityId;
	destination->moveAmountThreshold = source->moveAmountThreshold;
}

static void clearConfiguration(ShovelerViewTileSpriteAnimationConfiguration *configuration)
{
	configuration->tileSpriteEntityId = 0;
	configuration->moveAmountThreshold = 0.0f;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	ShovelerViewEntity *tileSpriteEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.tileSpriteEntityId);
	assert(tileSpriteEntity != NULL);
	const ShovelerViewTileSpriteConfiguration *tileSpriteConfiguration = shovelerViewEntityGetTileSpriteConfiguration(tileSpriteEntity);
	assert(tileSpriteConfiguration != NULL);
	ShovelerCanvasTileSprite *tileSprite = shovelerViewEntityGetTileSprite(tileSpriteEntity);
	assert(tileSprite != NULL);

	componentData->animation = shovelerTileSpriteAnimationCreate(tileSprite, componentData->configuration.moveAmountThreshold);
	componentData->animation->logDirectionChanges = true;

	componentData->positionMappingX = tileSpriteConfiguration->positionMappingX;
	componentData->positionMappingY = tileSpriteConfiguration->positionMappingY;

	const ShovelerViewPosition *position = shovelerViewEntityGetPosition(component->entity);
	componentData->previousPositionX = (float) shovelerViewPositionMapTileSpriteCoordinate(position, componentData->positionMappingX);
	componentData->previousPositionX = (float) shovelerViewPositionMapTileSpriteCoordinate(position, componentData->positionMappingY);

	componentData->positionCallback = shovelerViewEntityAddCallback(component->entity, shovelerViewPositionComponentName, &positionCallback, componentData);

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	free(componentData->animation);
	componentData->animation = NULL;

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, componentData->positionCallback);
	componentData->positionCallback = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	free(componentData->animation);

	if(componentData->positionCallback != NULL) {
		shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, componentData->positionCallback);
	}

	clearConfiguration(&componentData->configuration);
	free(componentData);
}

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	updatePosition(componentData, shovelerViewEntityGetPosition(positionComponent->entity));
}

static void updatePosition(ComponentData *componentData, const ShovelerViewPosition *position)
{
	float positionX = (float) shovelerViewPositionMapTileSpriteCoordinate(position, componentData->positionMappingX);
	float positionY = (float) shovelerViewPositionMapTileSpriteCoordinate(position, componentData->positionMappingY);

	float moveAmountX = positionX - componentData->previousPositionX;
	float moveAmountY = positionY - componentData->previousPositionY;

	shovelerTileSpriteAnimationUpdate(componentData->animation, shovelerVector2(moveAmountX, moveAmountY));

	componentData->previousPositionX = positionX;
	componentData->previousPositionY = positionY;
}
