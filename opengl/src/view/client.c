#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/client.h"
#include "shoveler/view/controller.h"
#include "shoveler/view/model.h"
#include "shoveler/view/position.h"
#include "shoveler/component.h"
#include "shoveler/controller.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/types.h"
#include "shoveler/view.h"

static void *activateClientComponent(ShovelerComponent *component);
static void deactivateClientComponent(ShovelerComponent *component);
static void moveController(ShovelerController *controller, ShovelerVector3 position, void *componentPointer);

ShovelerComponent *shovelerViewEntityAddClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewClientComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewClientComponentTypeName, activateClientComponent, deactivateClientComponent, /* requiresAuthority */ true);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewClientPositionOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewClientModelOptionKey, shovelerViewModelComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewClientComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewClientPositionOptionKey, configuration->positionEntityId);
	if(configuration->modelEntityId != 0) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewClientModelOptionKey, configuration->modelEntityId);
	}

	shovelerComponentActivate(component);
	return component;
}

bool shovelerViewEntityGetClientConfiguration(ShovelerViewEntity *entity, ShovelerViewClientConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentTypeName);
	if(component == NULL) {
		return false;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewClientPositionOptionKey);
	if(shovelerComponentHasConfigurationValue(component, shovelerViewClientModelOptionKey)) {
		outputConfiguration->modelEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewClientModelOptionKey);
	}
	return true;
}

bool shovelerViewEntityUpdateClient(ShovelerViewEntity *entity, const ShovelerViewClientConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update client of entity %lld which does not have a client, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewClientPositionOptionKey, configuration->positionEntityId);
	if(configuration->modelEntityId != 0) {
		shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewClientModelOptionKey, configuration->modelEntityId);
	}
}

bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewClientComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove client from entity %lld which does not have a client, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewClientComponentTypeName);
}

static void *activateClientComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasController(component->entity->view));

	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewClientPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);

	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	ShovelerReferenceFrame frame = controller->frame;
	frame.position = *positionCoordinates;
	shovelerControllerSetFrame(controller, &frame);

	ShovelerControllerMoveCallback *moveCallback = shovelerControllerAddMoveCallback(controller, moveController, component);

	if(shovelerComponentHasConfigurationValue(component, shovelerViewClientModelOptionKey)) {
		long long int modelEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewClientModelOptionKey);
		ShovelerViewEntity *modelEntity = shovelerViewGetEntity(component->entity->view, modelEntityId);
		assert(modelEntity != NULL);
		ShovelerModel *model = shovelerViewEntityGetModel(modelEntity);
		assert(model != NULL);

		model->visible = false;
	}

	return moveCallback;
}

static void deactivateClientComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasController(component->entity->view));

	ShovelerControllerMoveCallback *moveCallback = component->data;
	ShovelerController *controller = shovelerViewGetController(component->entity->view);
	shovelerControllerRemoveMoveCallback(controller, moveCallback);

	if(shovelerComponentHasConfigurationValue(component, shovelerViewClientModelOptionKey)) {
		long long int modelEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewClientModelOptionKey);
		ShovelerViewEntity *modelEntity = shovelerViewGetEntity(component->entity->view, modelEntityId);
		assert(modelEntity != NULL);
		ShovelerModel *model = shovelerViewEntityGetModel(modelEntity);
		assert(model != NULL);

		model->visible = true;
	}
}

static void moveController(ShovelerController *controller, ShovelerVector3 position, void *componentPointer)
{
	ShovelerComponent *component = componentPointer;

	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewClientPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	ShovelerComponent *positionComponent = shovelerViewEntityGetPositionComponent(positionEntity);
	assert(positionComponent != NULL);

	shovelerComponentUpdateConfigurationOptionVector3(positionComponent, shovelerViewPositionCoordinatesOptionKey, position);
}
