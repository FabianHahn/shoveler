#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/view.h"

bool shovelerViewSetResources(ShovelerView *view, ShovelerResources *resources)
{
	return shovelerViewSetTarget(view, shovelerViewResourcesTargetName, resources);
}

ShovelerResources *shovelerViewGetResources(ShovelerView *view)
{
	return (ShovelerResources *) shovelerViewGetTarget(view, shovelerViewResourcesTargetName);
}

ShovelerResources *shovelerComponentGetViewResources(ShovelerComponent *component)
{
	return (ShovelerResources *) shovelerComponentGetViewTarget(component, shovelerViewResourcesTargetName);
}
