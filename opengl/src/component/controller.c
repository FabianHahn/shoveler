#include "shoveler/component/controller.h"

#include "shoveler/component.h"

ShovelerController *shovelerComponentGetViewController(ShovelerComponent *component)
{
	return (ShovelerController *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetNameController);
}
