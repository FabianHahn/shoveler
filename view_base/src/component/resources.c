#include "shoveler/component/resources.h"

#include "shoveler/component.h"
#include "shoveler/resources.h"

const char *const shovelerComponentViewTargetIdResources = "resources";

ShovelerResources *shovelerComponentGetViewResources(ShovelerComponent *component)
{
	return (ShovelerResources *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdResources);
}
