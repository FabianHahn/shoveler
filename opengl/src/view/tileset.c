#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/resources.h"
#include "shoveler/view/tileset.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilesetConfiguration configuration;
	ShovelerTileset *tileset;
} TilesetComponentData;

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddTileset(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tileset to entity %lld which already has a tileset, ignoring.", entity->entityId);
		return false;
	}

	TilesetComponentData *componentData = malloc(sizeof(TilesetComponentData));
	componentData->configuration = configuration;
	componentData->tileset = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilesetComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, configuration.imageResourceEntityId, shovelerViewResourceComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentName);
	if(component == NULL) {
		return NULL;
	}

	TilesetComponentData *componentData = component->data;
	return componentData->tileset;
}

const ShovelerViewTilesetConfiguration *shovelerViewEntityGetTilesetConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentName);
	if(component == NULL) {
		return NULL;
	}

	TilesetComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTileset(ShovelerViewEntity *entity, ShovelerViewTilesetConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tileset of entity %lld which does not have a tileset, ignoring.", entity->entityId);
		return false;
	}

	TilesetComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.imageResourceEntityId, shovelerViewResourceComponentName)) {
		return false;
	}

	componentData->configuration = configuration;
	shovelerViewComponentAddDependency(component, componentData->configuration.imageResourceEntityId, shovelerViewResourceComponentName);
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTileset(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tileset from entity %lld which does not have a tileset, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilesetComponentName);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TilesetComponentData *componentData = componentDataPointer;

	ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.imageResourceEntityId);
	assert(imageResourceEntity != NULL);
	ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
	assert(image != NULL);

	componentData->tileset = shovelerTilesetCreate(image, componentData->configuration.columns, componentData->configuration.rows, componentData->configuration.padding);
	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TilesetComponentData *componentData = componentDataPointer;
	shovelerTilesetFree(componentData->tileset);
	componentData->tileset = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TilesetComponentData *componentData = componentDataPointer;
	shovelerTilesetFree(componentData->tileset);
	free(componentData);
}
