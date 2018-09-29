#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/client.h"
#include "shoveler/view/controller.h"
#include "shoveler/view/model.h"
#include "shoveler/view/position.h"
#include "shoveler/controller.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

typedef struct {
	ShovelerViewEntity *entity;
	double x;
	double y;
	double z;
	ShovelerControllerMoveCallback *moveCallback;
} ClientComponentData;

static void moveCallback(ShovelerController *controller, ShovelerVector3 position, void *clientComponentDataPointer);
static bool activateComponent(ShovelerViewComponent *component, void *clientComponentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *clientComponentDataPointer);

bool shovelerViewAddEntityClient(ShovelerView *view, long long int entityId)
{
	assert(shovelerViewHasController(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to add client to non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add client to entity %lld which already has a client, ignoring.", entityId);
		return false;
	}

	ClientComponentData *clientComponentData = malloc(sizeof(ClientComponentData));
	clientComponentData->entity = entity;
	clientComponentData->x = 0;
	clientComponentData->y = 0;
	clientComponentData->z = 0;
	clientComponentData->moveCallback = NULL;

	if (!shovelerViewEntityAddComponent(entity, shovelerViewClientComponentName, clientComponentData, activateComponent, NULL, freeComponent)) {
		free(clientComponentData);
		return false;
	}

	if(!shovelerViewEntityAddComponentDependency(entity, shovelerViewClientComponentName, entity->entityId, shovelerViewPositionComponentName)) {
		shovelerViewEntityRemoveComponent(entity, shovelerViewClientComponentName);
		return false;
	}

	if(!shovelerViewEntityAddComponentDependency(entity, shovelerViewClientComponentName, entity->entityId, shovelerViewModelComponentName)) {
		shovelerViewEntityRemoveComponent(entity, shovelerViewClientComponentName);
		return false;
	}

	shovelerViewEntityActivateComponent(entity, shovelerViewClientComponentName);
	return true;
}

bool shovelerViewDelegateClient(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to delegate client on existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to delegate client on entity %lld which does not have a client, ignoring.", entityId);
		return false;
	}
	ClientComponentData *clientComponentData = component->data;

	ShovelerController *controller = shovelerViewGetController(view);
	clientComponentData->moveCallback = shovelerControllerAddMoveCallback(controller, moveCallback, clientComponentData);

	return shovelerViewDelegateComponent(entity, shovelerViewClientComponentName);
}

bool shovelerViewUndelegateClient(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to undelegate client on existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to undelegate client on entity %lld which does not have a client, ignoring.", entityId);
		return false;
	}
	ClientComponentData *clientComponentData = component->data;

	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	shovelerControllerRemoveMoveCallback(controller, clientComponentData->moveCallback);
	clientComponentData->moveCallback = NULL;

	return shovelerViewUndelegateComponent(entity, shovelerViewClientComponentName);
}

bool shovelerViewRemoveEntityClient(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to remove client from non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove client from entity %lld which does not have a client, ignoring.", entityId);
		return false;
	}
	ClientComponentData *clientComponentData = component->data;

	return shovelerViewEntityRemoveComponent(entity, shovelerViewClientComponentName);
}

static void moveCallback(ShovelerController *controller, ShovelerVector3 position, void *clientComponentDataPointer)
{
	ClientComponentData *clientComponentData = clientComponentDataPointer;

	clientComponentData->x = position.values[0];
	clientComponentData->y = position.values[1];
	clientComponentData->z = position.values[2];

	shovelerViewRequestPositionUpdate(clientComponentData->entity->view, clientComponentData->entity->entityId, clientComponentData->x, clientComponentData->y, clientComponentData->z);
}

static bool activateComponent(ShovelerViewComponent *component, void *clientComponentDataPointer)
{
	ClientComponentData *clientComponentData = clientComponentDataPointer;

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(component->entity);
	clientComponentData->x = position->x;
	clientComponentData->y = position->y;
	clientComponentData->z = position->z;

	ShovelerModel *model = shovelerViewEntityGetModel(component->entity);
	model->visible = false;
	return true;
}

static void freeComponent(ShovelerViewComponent *component, void *clientComponentDataPointer)
{
	assert(shovelerViewHasController(component->entity->view));

	ClientComponentData *clientComponentData = clientComponentDataPointer;

	if(clientComponentData->moveCallback != NULL) {
		ShovelerController *controller = shovelerViewGetController(component->entity->view);
		shovelerControllerRemoveMoveCallback(controller, clientComponentData->moveCallback);
	}

	free(clientComponentData);
}
