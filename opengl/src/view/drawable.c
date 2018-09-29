#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/drawable/tiles.h"
#include "shoveler/view/drawable.h"
#include "shoveler/log.h"
#include "shoveler/view.h"

typedef struct {
	long long int entityId;
	ShovelerDrawable *drawable;
} DrawableComponentData;

static ShovelerDrawable *createDrawable(ShovelerViewDrawableConfiguration configuration);
static void freeComponent(ShovelerViewComponent *drawableComponent, void *drawableComponentDataPointer);
static void freeComponentData(DrawableComponentData *drawableComponentData);

bool shovelerViewAddEntityDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration configuration)
{
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

	DrawableComponentData *drawableComponentData = malloc(sizeof(DrawableComponentData));
	drawableComponentData->entityId = entityId;
	drawableComponentData->drawable = createDrawable(configuration);

	if(!shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentName, drawableComponentData, NULL, NULL, freeComponent)) {
		freeComponentData(drawableComponentData);
		return false;
	}

	shovelerViewEntityActivateComponent(entity, shovelerViewDrawableComponentName);
	return true;
}

ShovelerDrawable *shovelerViewGetEntityDrawable(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		return NULL;
	}

	return shovelerViewEntityGetDrawable(entity);
}

ShovelerDrawable *shovelerViewEntityGetDrawable(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component == NULL) {
		return NULL;
	}

	DrawableComponentData *drawableComponentData = component->data;
	return drawableComponentData->drawable;
}

bool shovelerViewUpdateEntityDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration configuration)
{
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

	DrawableComponentData *drawableComponentData = component->data;
	shovelerDrawableFree(drawableComponentData->drawable);
	drawableComponentData->drawable = createDrawable(configuration);

	return true;
}

bool shovelerViewRemoveEntityDrawable(ShovelerView *view, long long int entityId)
{
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

static void freeComponent(ShovelerViewComponent *drawableComponent, void *drawableComponentDataPointer)
{
	freeComponentData(drawableComponentDataPointer);
}

static void freeComponentData(DrawableComponentData *drawableComponentData)
{
	shovelerDrawableFree(drawableComponentData->drawable);
	free(drawableComponentData);
}
