#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/texture.h"
#include "shoveler/view/tileset.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilesetConfiguration configuration;
	ShovelerMaterialTilemapTileset tileset;
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
	componentData->tileset.columns = 0;
	componentData->tileset.rows = 0;
	componentData->tileset.texture = NULL;
	componentData->tileset.sampler = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilesetComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, configuration.textureEntityId, shovelerViewTextureComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerMaterialTilemapTileset *shovelerViewEntityGetTileset(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilesetComponentName);
	if(component == NULL) {
		return NULL;
	}

	TilesetComponentData *componentData = component->data;
	return &componentData->tileset;
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

	if(!shovelerViewComponentRemoveDependency(component, configuration.textureEntityId, shovelerViewTextureComponentName)) {
		return false;
	}

	componentData->configuration = configuration;
	shovelerViewComponentAddDependency(component, configuration.textureEntityId, shovelerViewTextureComponentName);
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

	ShovelerViewEntity *textureEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.textureEntityId);
	assert(textureEntity != NULL);
	ShovelerTexture *texture = shovelerViewEntityGetTexture(textureEntity);
	assert(texture != NULL);
	ShovelerSampler *sampler = shovelerViewEntityGetTextureSampler(textureEntity);
	assert(sampler != NULL);

	componentData->tileset.columns = componentData->configuration.columns;
	componentData->tileset.rows = componentData->configuration.rows;
	componentData->tileset.texture = texture;
	componentData->tileset.sampler = sampler;
	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TilesetComponentData *componentData = componentDataPointer;

	componentData->tileset.columns = 0;
	componentData->tileset.rows = 0;
	componentData->tileset.texture = NULL;
	componentData->tileset.sampler = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	TilesetComponentData *componentData = componentDataPointer;
	free(componentData);
}
