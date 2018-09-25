#include <stdlib.h> // malloc free

#include "shoveler/view/position.h"
#include "shoveler/log.h"

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

	ShovelerViewPosition *position = malloc(sizeof(ShovelerViewPosition));
	position->x = x;
	position->y = y;
	position->z = z;
	position->requestUpdate = NULL;
	position->requestUpdateUserData = NULL;

	if (!shovelerViewEntityAddComponent(entity, shovelerViewPositionComponentName, position, NULL, NULL, &freeComponent)) {
		freeComponent(component);
		return false;
	}
	return true;
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
	ShovelerViewPosition *position = component->data;

	position->x = x;
	position->y = y;
	position->z = z;

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
	ShovelerViewPosition *position = component->data;

	position->requestUpdate = requestUpdateFunction;
	position->requestUpdateUserData = userData;

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
	ShovelerViewPosition *position = component->data;

	position->requestUpdate = NULL;
	position->requestUpdateUserData = NULL;

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
	ShovelerViewPosition *position = component->data;

	if (!component->authoritative) {
		shovelerLogWarning("Trying to request position update for entity %lld for which this worker is not authoritative, ignoring.", entityId);
		return false;
	}

	position->requestUpdate(component, x, y, z, position->requestUpdateUserData);
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
	ShovelerViewPosition *position = component->data;
	free(position);
}
