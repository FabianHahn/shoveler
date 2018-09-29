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
	ShovelerDrawable *drawable;
} DrawableComponentData;

static ShovelerDrawable *createDrawable(ShovelerViewDrawableConfiguration configuration);
static void freeComponent(ShovelerViewComponent *drawableComponent, void *drawableComponentDataPointer);
static void freeComponentData(DrawableComponentData *drawableComponentData);

bool shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add drawable to entity %lld which already has a drawable, ignoring.", entity->entityId);
		return false;
	}

	DrawableComponentData *drawableComponentData = malloc(sizeof(DrawableComponentData));
	drawableComponentData->drawable = createDrawable(configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentName, drawableComponentData, NULL, NULL, freeComponent);
	assert(component != NULL);

	shovelerViewComponentActivate(component);
	return true;
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

bool shovelerViewEntityUpdateDrawable(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update drawable of entity %lld which does not have a drawable, ignoring.", entity->entityId);
		return false;
	}

	DrawableComponentData *drawableComponentData = component->data;
	shovelerDrawableFree(drawableComponentData->drawable);
	drawableComponentData->drawable = createDrawable(configuration);

	return true;
}

bool shovelerViewEntityRemoveDrawable(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove drawable from entity %lld which does not have a drawable, ignoring.", entity->entityId);
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
