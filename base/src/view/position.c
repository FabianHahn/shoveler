#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/position.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewPosition position;
	ShovelerViewPositionRequestUpdateFunction *requestUpdate;
	void *requestUpdateUserData;
} PositionComponentData;

static void freeComponent(ShovelerViewComponent *component, void *positionComponentDataPointer);

bool shovelerViewEntityAddPosition(ShovelerViewEntity *entity, double x, double y, double z)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add position to entity %lld which already has a position, ignoring.", entity->entityId);
		return false;
	}

	PositionComponentData *positionComponentData = malloc(sizeof(PositionComponentData));
	positionComponentData->position.x = x;
	positionComponentData->position.y = y;
	positionComponentData->position.z = z;
	positionComponentData->requestUpdate = NULL;
	positionComponentData->requestUpdateUserData = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewPositionComponentName, positionComponentData, NULL, NULL, freeComponent);
	assert(component != NULL);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerViewPosition *shovelerViewEntityGetPosition(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		return NULL;
	}

	PositionComponentData *positionComponentData = component->data;
	return &positionComponentData->position;
}

bool shovelerViewEntityUpdatePosition(ShovelerViewEntity *entity, double x, double y, double z)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update position for entity %lld which does not have a position, ignoring.", entity->entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->position.x = x;
	positionComponentData->position.y = y;
	positionComponentData->position.z = z;

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityDelegatePosition(ShovelerViewEntity *entity, ShovelerViewPositionRequestUpdateFunction *requestUpdateFunction, void *userData)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to delegate position for entity %lld which does not have a position, ignoring.", entity->entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->requestUpdate = requestUpdateFunction;
	positionComponentData->requestUpdateUserData = userData;

	shovelerViewComponentDelegate(component);
	return true;
}

bool shovelerViewEntityUndelegatePosition(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to undelegate position for entity %lld which does not have a position, ignoring.", entity->entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->requestUpdate = NULL;
	positionComponentData->requestUpdateUserData = NULL;

	shovelerViewComponentUndelegate(component);
	return true;
}

bool shovelerViewEntityRequestPositionUpdate(ShovelerViewEntity *entity, double x, double y, double z)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to request position update for entity %lld which does not have a position, ignoring.", entity->entityId);
		return false;
	}

	if (!component->authoritative) {
		shovelerLogWarning("Trying to request position update for entity %lld for which this worker is not authoritative, ignoring.", entity->entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->requestUpdate(component, x, y, z, positionComponentData->requestUpdateUserData);

	return true;
}

bool shovelerViewEntityRemoveEntityPosition(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove position from entity %lld which does not have a position, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewPositionComponentName);
}


static void freeComponent(ShovelerViewComponent *component, void *positionComponentDataPointer)
{
	PositionComponentData *positionComponentData = positionComponentDataPointer;
	free(positionComponentData);
}
