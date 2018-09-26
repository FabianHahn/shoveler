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
	ShovelerLight *light;
	ShovelerViewComponentCallback *positionCallback;
} LightComponentData;

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *lightComponentDataPointer);
static void freeComponent(ShovelerViewComponent *component);

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

	ShovelerLight *light;
	switch(lightConfiguration.type) {
		case SHOVELER_VIEW_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return false;
			break;
		case SHOVELER_VIEW_LIGHT_TYPE_POINT:
			light = shovelerLightPointCreate((ShovelerVector3) {0.0f, 0.0f, 0.0f}, lightConfiguration.width, lightConfiguration.height, lightConfiguration.samples, lightConfiguration.ambientFactor, lightConfiguration.exponentialFactor, lightConfiguration.color);
			break;
		default:
			shovelerLogWarning("Trying to create light with unknown light type %d, ignoring.", lightConfiguration.type);
			return false;
	}

	ShovelerScene *scene = shovelerViewGetScene(view);
	shovelerSceneAddLight(scene, light);

	LightComponentData *lightComponentData = malloc(sizeof(LightComponentData));
	lightComponentData->light = light;
	lightComponentData->positionCallback = shovelerViewEntityAddCallback(entity, shovelerViewPositionComponentName, &positionCallback, lightComponentData);

	if (!shovelerViewEntityAddComponent(entity, shovelerViewLightComponentName, lightComponentData, NULL, NULL, &freeComponent)) {
		freeComponent(component);
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
	ShovelerViewPosition *position = positionComponent->data;
	LightComponentData *lightComponentData = lightComponentDataPointer;
	shovelerLightUpdatePosition(lightComponentData->light, (ShovelerVector3){position->x, position->y, position->z});
}

static void freeComponent(ShovelerViewComponent *component)
{
	assert(shovelerViewHasScene(component->entity->view));

	LightComponentData *lightComponentData = component->data;

	ShovelerLight *light = lightComponentData->light;

	if(light != NULL) {
		ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
		shovelerSceneRemoveLight(scene, light);
	}

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, lightComponentData->positionCallback);

	free(lightComponentData);
}
