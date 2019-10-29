#include "shoveler/view/canvas.h"

#include "shoveler/view/tile_sprite.h"
#include "shoveler/component.h"
#include "shoveler/log.h"

ShovelerComponent *shovelerViewEntityAddCanvas(ShovelerViewEntity *entity, const ShovelerViewCanvasConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewCanvasComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateCanvasType());
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
		shovelerLogWarning("Trying to update canvas of entity %lld which does not have a canvas, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityIdArray(component, shovelerViewCanvasTileSpritesOptionKey, configuration->tileSpriteEntityIds, configuration->numTileSprites);
	return true;
}

bool shovelerViewEntityRemoveCanvas(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewCanvasComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove canvas from entity %lld which does not have a canvas, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewCanvasComponentTypeName);
}
