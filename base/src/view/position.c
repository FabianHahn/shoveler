#include <stdlib.h> // malloc free

#include "shoveler/view/position.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewPosition position;
	ShovelerViewPositionRequestUpdateFunction *requestUpdate;
	void *requestUpdateUserData;
} PositionComponentData;

static void freeComponent(ShovelerViewComponent *component);

bool shovelerViewAddEntityPosition(ShovelerView *view, long long int entityId, double x, double y, double z)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to add position to non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add position to entity %lld which already has a position, ignoring.", entityId);
		return false;
	}

	PositionComponentData *positionComponentData = malloc(sizeof(PositionComponentData));
	positionComponentData->position.x = x;
	positionComponentData->position.y = y;
	positionComponentData->position.z = z;
	positionComponentData->requestUpdate = NULL;
	positionComponentData->requestUpdateUserData = NULL;

	if (!shovelerViewEntityAddComponent(entity, shovelerViewPositionComponentName, positionComponentData, NULL, NULL, &freeComponent)) {
		freeComponent(component);
		return false;
	}

	shovelerViewEntityActivateComponent(entity, shovelerViewPositionComponentName);
	return true;
}

ShovelerViewPosition *shovelerViewGetEntityPosition(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		return NULL;
	}

	return shovelerViewEntityGetPosition(entity);
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

bool shovelerViewUpdateEntityPosition(ShovelerView *view, long long int entityId, double x, double y, double z)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update position for non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update position for entity %lld which does not have a position, ignoring.", entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->position.x = x;
	positionComponentData->position.y = y;
	positionComponentData->position.z = z;

	return shovelerViewEntityUpdateComponent(entity, shovelerViewPositionComponentName);
}

bool shovelerViewDelegatePosition(ShovelerView *view, long long int entityId, ShovelerViewPositionRequestUpdateFunction *requestUpdateFunction, void *userData)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to delegate position for non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to delegate position for entity %lld which does not have a position, ignoring.", entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->requestUpdate = requestUpdateFunction;
	positionComponentData->requestUpdateUserData = userData;

	return shovelerViewDelegateComponent(entity, shovelerViewPositionComponentName);
}

bool shovelerViewUndelegatePosition(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to undelegate position for non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to undelegate position for entity %lld which does not have a position, ignoring.", entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->requestUpdate = NULL;
	positionComponentData->requestUpdateUserData = NULL;

	return shovelerViewUndelegateComponent(entity, shovelerViewPositionComponentName);
}

bool shovelerViewRequestPositionUpdate(ShovelerView *view, long long int entityId, double x, double y, double z)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to request position update for non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to request position update for entity %lld which does not have a position, ignoring.", entityId);
		return false;
	}

	if (!component->authoritative) {
		shovelerLogWarning("Trying to request position update for entity %lld for which this worker is not authoritative, ignoring.", entityId);
		return false;
	}

	PositionComponentData *positionComponentData = component->data;
	positionComponentData->requestUpdate(component, x, y, z, positionComponentData->requestUpdateUserData);

	return true;
}

bool shovelerViewRemoveEntityPosition(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to remove position from non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove position from entity %lld which does not have a position, ignoring.", entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewPositionComponentName);
}


static void freeComponent(ShovelerViewComponent *component)
{
	PositionComponentData *positionComponentData = component->data;
	free(positionComponentData);
}
