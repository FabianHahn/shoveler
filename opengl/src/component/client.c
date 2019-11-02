#include "shoveler/component/client.h"

#include <assert.h> // assert

#include "shoveler/component/controller.h"
#include "shoveler/component/model.h"
#include "shoveler/component/position.h"
#include "shoveler/component.h"
#include "shoveler/controller.h"
#include "shoveler/model.h"

const char *const shovelerComponentTypeIdClient = "client";

static void *activateClientComponent(ShovelerComponent *component);
static void deactivateClientComponent(ShovelerComponent *component);
static void moveController(ShovelerController *controller, ShovelerVector3 position, void *componentPointer);

ShovelerComponentType *shovelerComponentCreateClientType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdClient, activateClientComponent, deactivateClientComponent, /* requiresAuthority */ true);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentClientOptionKeyPosition, shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentClientOptionKeyModel, shovelerComponentTypeIdModel, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}

static void *activateClientComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewController(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerComponentClientOptionKeyPosition);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);

	ShovelerController *controller = shovelerComponentGetViewController(component);
	ShovelerReferenceFrame frame = controller->frame;
	frame.position = *positionCoordinates;
	shovelerControllerSetFrame(controller, &frame);

	ShovelerControllerMoveCallback *moveCallback = shovelerControllerAddMoveCallback(controller, moveController, component);

	if(shovelerComponentHasConfigurationValue(component, shovelerComponentClientOptionKeyModel)) {
		ShovelerComponent *modelComponent = shovelerComponentGetDependency(component, shovelerComponentClientOptionKeyModel);
		assert(modelComponent != NULL);
		ShovelerModel *model = shovelerComponentGetModel(modelComponent);
		assert(model != NULL);

		model->visible = false;
	}

	return moveCallback;
}

static void deactivateClientComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewController(component));

	ShovelerControllerMoveCallback *moveCallback = component->data;
	ShovelerController *controller = shovelerComponentGetViewController(component);
	shovelerControllerRemoveMoveCallback(controller, moveCallback);

	if(shovelerComponentHasConfigurationValue(component, shovelerComponentClientOptionKeyModel)) {
		ShovelerComponent *modelComponent = shovelerComponentGetDependency(component, shovelerComponentClientOptionKeyModel);
		assert(modelComponent != NULL);
		ShovelerModel *model = shovelerComponentGetModel(modelComponent);
		assert(model != NULL);

		model->visible = true;
	}
}

static void moveController(ShovelerController *controller, ShovelerVector3 position, void *componentPointer)
{
	ShovelerComponent *component = componentPointer;

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerComponentClientOptionKeyPosition);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionComponent != NULL);

	shovelerComponentUpdateConfigurationOptionVector3(positionComponent, shovelerComponentPositionOptionKeyCoordinates, position);
}
