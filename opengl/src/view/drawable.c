#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/drawable/tiles.h"
#include "shoveler/view/drawable.h"
#include "shoveler/view/drawables.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

static ShovelerDrawable *createDrawable(ShovelerViewDrawableConfiguration configuration);
static void freeComponent(ShovelerViewComponent *drawableComponent);
static void freeComponentData(ShovelerViewDrawable *drawableComponentData);

bool shovelerViewAddEntityDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration configuration)
{
	assert(shovelerViewHasDrawables(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to add drawable to non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add drawable to entity %lld which already has a drawable, ignoring.", entityId);
		return false;
	}

	ShovelerViewDrawable *drawableComponentData = malloc(sizeof(ShovelerViewDrawable));
	drawableComponentData->entityId = entityId;
	drawableComponentData->drawable = createDrawable(configuration);

	ShovelerViewDrawables *drawables = shovelerViewGetDrawables(view);
	if(!g_hash_table_insert(drawables->entities, &drawableComponentData->entityId, drawableComponentData->drawable)) {
		freeComponentData(drawableComponentData);
		return false;
	}

	if(!shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentName, drawableComponentData, &freeComponent)) {
		freeComponentData(drawableComponentData);
		return false;
	}
	return true;
}

bool shovelerViewUpdateEntityDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration configuration)
{
	assert(shovelerViewHasDrawables(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update drawable of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update drawable of entity %lld which does not have a drawable, ignoring.", entityId);
		return false;
	}
	ShovelerViewDrawable *drawableComponentData = component->data;

	shovelerDrawableFree(drawableComponentData->drawable);
	drawableComponentData->drawable = createDrawable(configuration);

	ShovelerViewDrawables *drawables = shovelerViewGetDrawables(view);
	if(!g_hash_table_replace(drawables->entities, &drawableComponentData->entityId, drawableComponentData->drawable)) {
		freeComponentData(drawableComponentData);
		return false;
	}

	return true;
}

bool shovelerViewRemoveEntityDrawable(ShovelerView *view, long long int entityId)
{
	assert(shovelerViewHasDrawables(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to remove drawable from non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove drawable from entity %lld which does not have a drawable, ignoring.", entityId);
		return false;
	}
	ShovelerViewDrawable *drawableComponentData = component->data;

	return shovelerViewEntityRemoveComponent(entity, shovelerViewDrawableComponentName);
}

static ShovelerDrawable *createDrawable(ShovelerViewDrawableConfiguration configuration)
{
	switch (configuration.type) {
		case SHOVELER_VIEW_DRAWABLE_TYPE_CUBE:
			return shovelerDrawableCubeCreate();
		case SHOVELER_VIEW_DRAWABLE_TYPE_QUAD:
			return shovelerDrawableQuadCreate();
		case SHOVELER_VIEW_DRAWABLE_TYPE_POINT:
			return shovelerDrawablePointCreate();
		case SHOVELER_VIEW_DRAWABLE_TYPE_TILES:
			return shovelerDrawableTilesCreate(configuration.tilesSize.width, configuration.tilesSize.height);
		default:
			shovelerLogWarning("Trying to create drawable with unknown type %d, ignoring.", configuration.type);
			return NULL;
	}
}

static void freeComponent(ShovelerViewComponent *drawableComponent)
{
	assert(shovelerViewHasDrawables(drawableComponent->entity->view));

	ShovelerViewDrawables *drawables = shovelerViewGetDrawables(drawableComponent->entity->view);
	g_hash_table_remove(drawables->entities, &drawableComponent->entity->entityId);

	freeComponentData(drawableComponent->data);
}

static void freeComponentData(ShovelerViewDrawable *drawableComponentData)
{
	shovelerDrawableFree(drawableComponentData->drawable);
	free(drawableComponentData);
}
