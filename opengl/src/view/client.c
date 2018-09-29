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

bool shovelerViewEntityAddClient(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasController(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add client to entity %lld which already has a client, ignoring.", entity->entityId);
		return false;
	}

	ClientComponentData *clientComponentData = malloc(sizeof(ClientComponentData));
	clientComponentData->entity = entity;
	clientComponentData->x = 0;
	clientComponentData->y = 0;
	clientComponentData->z = 0;
	clientComponentData->moveCallback = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewClientComponentName, clientComponentData, activateComponent, NULL, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);
	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewModelComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

bool shovelerViewEntityDelegateClient(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to delegate client on entity %lld which does not have a client, ignoring.", entity->entityId);
		return false;
	}
	ClientComponentData *clientComponentData = component->data;

	ShovelerController *controller = shovelerViewGetController(entity->view);
	clientComponentData->moveCallback = shovelerControllerAddMoveCallback(controller, moveCallback, clientComponentData);

	shovelerViewComponentDelegate(component);
	return true;
}

bool shovelerViewEntityUndelegateClient(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to undelegate client on entity %lld which does not have a client, ignoring.", entity->entityId);
		return false;
	}
	ClientComponentData *clientComponentData = component->data;

	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	shovelerControllerRemoveMoveCallback(controller, clientComponentData->moveCallback);
	clientComponentData->moveCallback = NULL;

	shovelerViewComponentUndelegate(component);
	return true;
}

bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove client from entity %lld which does not have a client, ignoring.", entity->entityId);
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

	shovelerViewEntityRequestPositionUpdate(clientComponentData->entity, clientComponentData->x, clientComponentData->y, clientComponentData->z);
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
