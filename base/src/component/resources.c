#include "shoveler/component/resources.h"

#include "shoveler/component.h"
#include "shoveler/resources.h"

ShovelerResources *shovelerComponentGetViewResources(ShovelerComponent *component)
{
	return (ShovelerResources *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetNameResources);
}
