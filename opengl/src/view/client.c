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
	ShovelerViewClientConfiguration configuration;
	ShovelerViewEntity *entity;
	ShovelerControllerMoveCallback *moveCallback;
} ComponentData;

static void assignConfiguration(ShovelerViewClientConfiguration *destination, const ShovelerViewClientConfiguration *source);
static void clearConfiguration(ShovelerViewClientConfiguration *configuration);
static void moveCallback(ShovelerController *controller, ShovelerVector3 position, void *componentDataPointer);
static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	assert(shovelerViewHasController(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add client to entity %lld which already has a client, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration.positionEntityId = 0;
	componentData->configuration.modelEntityId = 0;
	componentData->configuration.disableModelVisibility = false;
	componentData->entity = entity;
	componentData->moveCallback = NULL;

	assignConfiguration(&componentData->configuration, configuration);

	component = shovelerViewEntityAddComponent(entity, shovelerViewClientComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, componentData->configuration.positionEntityId, shovelerViewPositionComponentName);

	if(componentData->configuration.modelEntityId > 0) {
		shovelerViewComponentAddDependency(component, componentData->configuration.modelEntityId, shovelerViewModelComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

const ShovelerViewClientConfiguration *shovelerViewEntityGetClientConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		return false;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
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

bool shovelerViewEntityUpdateClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update client of entity %lld which does not have a client, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.modelEntityId, shovelerViewModelComponentName)) {
		return false;
	}

	if(!shovelerViewComponentRemoveDependency(component, componentData->configuration.positionEntityId, shovelerViewPositionComponentName)) {
		return false;
	}

	assignConfiguration(&componentData->configuration, configuration);

	shovelerViewComponentAddDependency(component, componentData->configuration.positionEntityId, shovelerViewPositionComponentName);

	if(componentData->configuration.modelEntityId > 0) {
		shovelerViewComponentAddDependency(component, componentData->configuration.modelEntityId, shovelerViewModelComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
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

static void assignConfiguration(ShovelerViewClientConfiguration *destination, const ShovelerViewClientConfiguration *source)
{
	clearConfiguration(destination);

	destination->positionEntityId = source->positionEntityId;
	destination->modelEntityId = source->modelEntityId;
	destination->disableModelVisibility = source->disableModelVisibility;
}

static void clearConfiguration(ShovelerViewClientConfiguration *configuration)
{
	configuration->positionEntityId = 0;
	configuration->modelEntityId = 0;
	configuration->disableModelVisibility = false;
}

static void moveCallback(ShovelerController *controller, ShovelerVector3 position, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(componentData->entity->view, componentData->configuration.positionEntityId);
	assert(positionEntity != NULL);
	shovelerViewEntityRequestPositionUpdate(positionEntity, position.values[0], position.values[1], position.values[2]);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	assert(shovelerViewHasController(component->entity->view));

	ComponentData *componentData = componentDataPointer;

	if(!component->authoritative) {
		shovelerLogWarning("Failed to activate client of entity %lld because it is not authoritative.", component->entity->entityId);
		return false;
	}

	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	componentData->moveCallback = shovelerControllerAddMoveCallback(controller, moveCallback, componentData);

	if(componentData->configuration.modelEntityId > 0) {
		ShovelerViewEntity *modelEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.modelEntityId);
		assert(modelEntity != NULL);
		ShovelerModel *model = shovelerViewEntityGetModel(modelEntity);
		assert(model != NULL);
		if (componentData->configuration.disableModelVisibility) {
			model->visible = false;
		}
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

	if(componentData->configuration.modelEntityId > 0) {
		ShovelerViewEntity *modelEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.modelEntityId);
		assert(modelEntity != NULL);
		ShovelerModel *model = shovelerViewEntityGetModel(modelEntity);
		assert(model != NULL);
		if (componentData->configuration.disableModelVisibility) {
			model->visible = true;
		}
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
