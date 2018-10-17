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
	ShovelerViewDrawableConfiguration configuration;
	ShovelerDrawable *drawable;
} ComponentData;

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddDrawable(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add drawable to entity %lld which already has a drawable, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration = configuration;
	componentData->drawable = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
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

	ComponentData *drawableComponentData = component->data;
	return drawableComponentData->drawable;
}

bool shovelerViewEntityUpdateDrawable(ShovelerViewEntity *entity, ShovelerViewDrawableConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewDrawableComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update drawable of entity %lld which does not have a drawable, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);
	componentData->configuration = configuration;
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
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


static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	switch (componentData->configuration.type) {
		case SHOVELER_VIEW_DRAWABLE_TYPE_CUBE:
			componentData->drawable = shovelerDrawableCubeCreate();
			break;
		case SHOVELER_VIEW_DRAWABLE_TYPE_QUAD:
			componentData->drawable = shovelerDrawableQuadCreate();
			break;
		case SHOVELER_VIEW_DRAWABLE_TYPE_POINT:
			componentData->drawable = shovelerDrawablePointCreate();
			break;
		case SHOVELER_VIEW_DRAWABLE_TYPE_TILES:
			componentData->drawable = shovelerDrawableTilesCreate(componentData->configuration.tilesSize.width, componentData->configuration.tilesSize.height);
			break;
		default:
			shovelerLogWarning("Failed to activate drawable with unknown type %d.", componentData->configuration.type);
			return false;
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerDrawableFree(componentData->drawable);
	componentData->drawable = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	if(componentData->drawable != NULL) {
		shovelerDrawableFree(componentData->drawable);
	}

	free(componentData);
}
