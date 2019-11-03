#include "shoveler/component/controller.h"

#include "shoveler/component.h"

const char *const shovelerComponentViewTargetIdController = "controller";

ShovelerController *shovelerComponentGetViewController(ShovelerComponent *component)
{
	return (ShovelerController *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdController);
}
