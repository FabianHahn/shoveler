#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <string.h> // memcpy

#include "shoveler/view/canvas.h"
#include "shoveler/view/tile_sprite.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

static void *activateCanvasComponent(ShovelerComponent *component);
static void deactivateCanvasComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewCanvasComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewCanvasComponentTypeName, activateCanvasComponent, deactivateCanvasComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewCanvasTileSpritesOptionKey, shovelerViewTileSpriteComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewCanvasComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewCanvasTileSpritesOptionKey, configuration->tileSpriteEntityIds, configuration->numTileSprites);

	shovelerComponentActivate(component);
	return component;
}

ShovelerCanvas *shovelerViewEntityGetCanvas(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetCanvasConfiguration(ShovelerViewEntity *entity, ShovelerViewCanvasConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentTypeName);
	if(component == NULL) {
		return false;
	}

	const ShovelerComponentConfigurationValue *tileSpritesValue = shovelerComponentGetConfigurationValue(component, shovelerViewCanvasTileSpritesOptionKey);
	assert(tileSpritesValue != NULL);

	outputConfiguration->tileSpriteEntityIds = tileSpritesValue->entityIdArrayValue.entityIds;
	outputConfiguration->numTileSprites = tileSpritesValue->entityIdArrayValue.size;
	return true;
}

bool shovelerViewEntityUpdateCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update canvas of entity %lld which does not have a canvas, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewCanvasTileSpritesOptionKey, configuration->tileSpriteEntityIds, configuration->numTileSprites);
	return true;
}

bool shovelerViewEntityRemoveCanvas(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas from entity %lld which does not have a canvas, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewCanvasComponentTypeName);
}

static void *activateCanvasComponent(ShovelerComponent *component)
{
	ShovelerCanvas *canvas = shovelerCanvasCreate();

	const ShovelerComponentConfigurationValue *tileSpritesValue = shovelerComponentGetConfigurationValue(component, shovelerViewCanvasTileSpritesOptionKey);
	assert(tileSpritesValue != NULL);

	for(size_t i = 0; i < tileSpritesValue->entityIdArrayValue.size; i++) {
		ShovelerViewEntity *tilesetEntity = shovelerViewGetEntity(component->entity->view, tileSpritesValue->entityIdArrayValue.entityIds[i]);
		assert(tilesetEntity != NULL);
		ShovelerCanvasTileSprite *tileSprite = shovelerViewEntityGetTileSprite(tilesetEntity);
		assert(tileSprite != NULL);

		shovelerCanvasAddTileSprite(canvas, tileSprite);
	}

	return canvas;
}

static void deactivateCanvasComponent(ShovelerComponent *component)
{
	shovelerCanvasFree(component->data);
}
