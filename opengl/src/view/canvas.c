#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/canvas.h"
#include "shoveler/view/canvas_tile_sprite.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewCanvasConfiguration configuration;
	ShovelerCanvas *canvas;
} ComponentData;

static void assignConfiguration(ShovelerViewCanvasConfiguration *destination, const ShovelerViewCanvasConfiguration *source);
static void clearConfiguration(ShovelerViewCanvasConfiguration *configuration);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add canvas to entity %lld which already has a canvas, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.numTileSprites = 0;
	componentData->configuration.tileSpriteEntityIds = NULL;
	componentData->canvas = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewCanvasComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	for(int i = 0; i < componentData->configuration.numTileSprites; i++) {
		shovelerViewComponentAddDependency(component, componentData->configuration.tileSpriteEntityIds[i], shovelerViewCanvasTileSpriteComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerCanvas *shovelerViewEntityGetCanvas(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentName);
	if(component == NULL) {
		return NULL;
	}

	ComponentData *componentData = component->data;
	return componentData->canvas;
}

const ShovelerViewCanvasConfiguration *shovelerViewEntityGetCanvasConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateCanvas(ShovelerViewEntity *entity, ShovelerViewCanvasConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas of entity %lld which does not have a canvas, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	for(int i = 0; i < componentData->configuration.numTileSprites; i++) {
		if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.tileSpriteEntityIds[i], shovelerViewCanvasTileSpriteComponentName)) {
			return false;
		}
	}

	assignConfiguration(&componentData->configuration, &configuration);

	for(int i = 0; i < componentData->configuration.numTileSprites; i++) {
		shovelerViewComponentAddDependency(component, componentData->configuration.tileSpriteEntityIds[i], shovelerViewCanvasTileSpriteComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveCanvas(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas from entity %lld which does not have a canvas, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewCanvasComponentName);
}

static void assignConfiguration(ShovelerViewCanvasConfiguration *destination, const ShovelerViewCanvasConfiguration *source)
{
	clearConfiguration(destination);

	destination->numTileSprites = source->numTileSprites;
	destination->tileSpriteEntityIds = malloc(destination->numTileSprites * sizeof(long long int));
	memcpy(destination->tileSpriteEntityIds, source->tileSpriteEntityIds, destination->numTileSprites * sizeof(long long int));
}

static void clearConfiguration(ShovelerViewCanvasConfiguration *configuration)
{
	if(configuration->numTileSprites > 0) {
		free(configuration->tileSpriteEntityIds);
		configuration->tileSpriteEntityIds = NULL;
	}
	configuration->numTileSprites = 0;
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	componentData->canvas = shovelerCanvasCreate();

	for(int i = 0; i < componentData->configuration.numTileSprites; i++) {
		ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.tileSpriteEntityIds[i]);
		assert(tilesetEntity != NULL);
		ShovelerCanvasTileSprite *tileSprite = shovelerViewEntityGetCanvasTileSprite(tilesetEntity);
		assert(tileSprite != NULL);

		shovelerCanvasAddTileSprite(componentData->canvas, tileSprite);
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerCanvasFree(componentData->canvas);
	componentData->canvas = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	clearConfiguration(&componentData->configuration);
	shovelerCanvasFree(componentData->canvas);
	free(componentData);
}
