#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/resources.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/view/tileset.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewTilemapConfiguration configuration;
	int numLayers;
	ShovelerTexture **layers;
	int numTilesets;
	ShovelerMaterialTilemapTileset **tilesets;
} ComponentData;

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddTilemap(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add tilemap to entity %lld which already has a tilemap, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration = configuration;
	componentData->numLayers = 0;
	componentData->layers = NULL;
	componentData->numTilesets = 0;
	componentData->tilesets = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewTilemapComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	for(int i = 0; i < configuration.numLayers; i++) {
		shovelerViewComponentAddDependency(component, configuration.layerImageResourceEntityIds[i], shovelerViewResourceComponentName);
	}
	for(int i = 0; i < configuration.numTilesets; i++) {
		shovelerViewComponentAddDependency(component, configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

bool shovelerViewEntityGetTilemapLayers(ShovelerViewEntity *entity, int *numLayersPointer, ShovelerTexture ***layersPointer)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	*numLayersPointer = componentData->numLayers;
	*layersPointer = componentData->layers;
	return true;
}

bool shovelerViewEntityGetTilemapTilesets(ShovelerViewEntity *entity, int *numTilesetsPointer, ShovelerMaterialTilemapTileset ***tilesetsPointer)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	*numTilesetsPointer = componentData->numLayers;
	*tilesetsPointer = componentData->tilesets;
	return true;
}

const ShovelerViewTilemapConfiguration *shovelerViewEntityGetTilemapConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateTilemap(ShovelerViewEntity *entity, ShovelerViewTilemapConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update tilemap of entity %lld which does not have a tilemap, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.layerImageResourceEntityIds[i], shovelerViewResourceComponentName)) {
			return false;
		}
	}
	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName)) {
			return false;
		}
	}

	componentData->configuration = configuration;

	for(int i = 0; i < configuration.numLayers; i++) {
		shovelerViewComponentAddDependency(component, configuration.layerImageResourceEntityIds[i], shovelerViewResourceComponentName);
	}
	for(int i = 0; i < configuration.numTilesets; i++) {
		shovelerViewComponentAddDependency(component, configuration.tilesetEntityIds[i], shovelerViewTilesetComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveTilemap(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewTilemapComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove tilemap from entity %lld which does not have a tilemap, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewTilemapComponentName);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	componentData->numLayers = componentData->configuration.numLayers;
	componentData->layers = malloc(componentData->numLayers * sizeof(ShovelerTexture *));

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		ShovelerViewEntity *imageResourceEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.layerImageResourceEntityIds[i]);
		assert(imageResourceEntity != NULL);
		ShovelerImage *image = shovelerViewEntityGetResource(imageResourceEntity);
		assert(image != NULL);

		componentData->layers[i] = shovelerTextureCreate2d(image, false);
		shovelerTextureUpdate(componentData->layers[i]);
	}

	componentData->numTilesets = componentData->configuration.numTilesets;
	componentData->tilesets = malloc(componentData->numTilesets * sizeof(ShovelerMaterialTilemapTileset *));

	for(int i = 0; i < componentData->configuration.numTilesets; i++) {
		ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.tilesetEntityIds[i]);
		assert(tilesetEntity != NULL);
		componentData->tilesets[i] = shovelerViewEntityGetTileset(tilesetEntity);
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		shovelerTextureFree(componentData->layers[i]);
	}
	free(componentData->layers);

	componentData->numLayers = 0;
	componentData->layers = NULL;

	free(componentData->tilesets);

	componentData->numTilesets = 0;
	componentData->tilesets = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	for(int i = 0; i < componentData->configuration.numLayers; i++) {
		shovelerTextureFree(componentData->layers[i]);
	}

	free(componentData->layers);
	free(componentData->tilesets);
	free(componentData);
}
