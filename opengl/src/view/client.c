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
	ShovelerControllerMoveCallback *moveCallback;
} ComponentData;

static void moveCallback(ShovelerController *controller, ShovelerVector3 position, void *componentDataPointer);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddClient(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasController(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add client to entity %lld which already has a client, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->entity = entity;
	componentData->moveCallback = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewClientComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);

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

	shovelerViewComponentDelegate(component);
	shovelerViewComponentActivate(component);
	return true;
}

bool shovelerViewEntityUndelegateClient(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to undelegate client on entity %lld which does not have a client, ignoring.", entity->entityId);
		return false;
	}

	shovelerViewComponentDeactivate(component);
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

	return shovelerViewEntityRemoveComponent(entity, shovelerViewClientComponentName);
}

static void moveCallback(ShovelerController *controller, ShovelerVector3 position, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerViewEntityRequestPositionUpdate(componentData->entity, position.values[0], position.values[1], position.values[2]);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasController(component->entity->view));

	ComponentData *componentData = componentDataPointer;

	if(component->authoritative) {
		shovelerLogWarning("Failed to activate client of entity %lld because it is not authoritative.", component->entity->entityId);
		return false;
	}

	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	componentData->moveCallback = shovelerControllerAddMoveCallback(controller, moveCallback, componentData);

	ShovelerModel *model = shovelerViewEntityGetModel(component->entity);
	if(model != NULL) {
		model->visible = false;
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasController(component->entity->view));

	ComponentData *componentData = componentDataPointer;

	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	shovelerControllerRemoveMoveCallback(controller, componentData->moveCallback);
	componentData->moveCallback = NULL;

	ShovelerModel *model = shovelerViewEntityGetModel(component->entity);
	if(model != NULL) {
		model->visible = true;
	}
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasController(component->entity->view));

	ComponentData *componentData = componentDataPointer;

	if(componentData->moveCallback != NULL) {
		ShovelerController *controller = shovelerViewGetController(component->entity->view);
		shovelerControllerRemoveMoveCallback(controller, componentData->moveCallback);
	}

	free(componentData);
}
