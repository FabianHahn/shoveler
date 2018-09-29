#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/light/point.h"
#include "shoveler/view/light.h"
#include "shoveler/view/position.h"
#include "shoveler/view/scene.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/scene.h"
#include "shoveler/view.h"

typedef struct {
	ShovelerViewLightConfiguration configuration;
	ShovelerLight *light;
	ShovelerViewComponentCallback *positionCallback;
} LightComponentData;

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *lightComponentDataPointer);
static bool activateComponent(ShovelerViewComponent *component, void *lightComponentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *lightComponentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *lightComponentDataPointer);

bool shovelerViewAddEntityLight(ShovelerView *view, long long int entityId, ShovelerViewLightConfiguration lightConfiguration)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to add light to non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add light to entity %lld which already has a light, ignoring.", entityId);
		return false;
	}

	LightComponentData *lightComponentData = malloc(sizeof(LightComponentData));
	lightComponentData->configuration = lightConfiguration;
	lightComponentData->light = NULL;
	lightComponentData->positionCallback = NULL;

	if (!shovelerViewEntityAddComponent(entity, shovelerViewLightComponentName, lightComponentData, activateComponent, deactivateComponent, freeComponent)) {
		free(lightComponentData);
		return false;
	}

	if(!shovelerViewEntityAddComponentDependency(entity, shovelerViewLightComponentName, entity->entityId, shovelerViewPositionComponentName)) {
		shovelerViewEntityRemoveComponent(entity, shovelerViewLightComponentName);
		return false;
	}

	shovelerViewEntityActivateComponent(entity, shovelerViewLightComponentName);
	return true;
}

bool shovelerViewRemoveEntityLight(ShovelerView *view, long long int entityId)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to remove light from non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove light from entity %lld which does not have a light, ignoring.", entityId);
		return false;
	}
	LightComponentData *lightComponentData = component->data;

	ShovelerScene *scene = shovelerViewGetScene(view);
	shovelerSceneRemoveLight(scene, lightComponentData->light);

	return shovelerViewEntityRemoveComponent(entity, shovelerViewLightComponentName);
}

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *lightComponentDataPointer)
{
	LightComponentData *lightComponentData = lightComponentDataPointer;

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(positionComponent->entity);
	shovelerLightUpdatePosition(lightComponentData->light, shovelerVector3(position->x, position->y, position->z));
}

static bool activateComponent(ShovelerViewComponent *component, void *lightComponentDataPointer)
{
	LightComponentData *lightComponentData = lightComponentDataPointer;
	assert(shovelerViewHasScene(component->entity->view));

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(component->entity);

	switch(lightComponentData->configuration.type) {
		case SHOVELER_VIEW_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return false;
		case SHOVELER_VIEW_LIGHT_TYPE_POINT:
			lightComponentData->light = shovelerLightPointCreate(shovelerVector3(position->x, position->y, position->z), lightComponentData->configuration.width, lightComponentData->configuration.height, lightComponentData->configuration.samples, lightComponentData->configuration.ambientFactor, lightComponentData->configuration.exponentialFactor, lightComponentData->configuration.color);
			break;
		default:
			shovelerLogWarning("Trying to create light with unknown light type %d, ignoring.", lightComponentData->configuration.type);
			return false;
	}

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneAddLight(scene, lightComponentData->light);

	lightComponentData->positionCallback = shovelerViewEntityAddCallback(component->entity, shovelerViewPositionComponentName, &positionCallback, lightComponentData);
	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *lightComponentDataPointer)
{
	LightComponentData *lightComponentData = lightComponentDataPointer;
	assert(shovelerViewHasScene(component->entity->view));

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneRemoveLight(scene, lightComponentData->light);
	lightComponentData->light = NULL;

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, lightComponentData->positionCallback);
	lightComponentData->positionCallback = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *lightComponentDataPointer)
{
	assert(shovelerViewHasScene(component->entity->view));

	LightComponentData *lightComponentData = component->data;

	ShovelerLight *light = lightComponentData->light;

	if(light != NULL) {
		ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
		shovelerSceneRemoveLight(scene, light);
	}

	if(lightComponentData != NULL) {
		shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, lightComponentData->positionCallback);
	}

	free(lightComponentData);
}
