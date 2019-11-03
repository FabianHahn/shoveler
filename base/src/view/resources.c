#include "shoveler/view/resources.h"

#include "shoveler/component/resources.h"
#include "shoveler/resources.h"
#include "shoveler/view.h"

bool shovelerViewSetResources(ShovelerView *view, ShovelerResources *resources)
{
	return shovelerViewSetTarget(view, shovelerComponentViewTargetIdResources, resources);
}

ShovelerResources *shovelerViewGetResources(ShovelerView *view)
{
	return (ShovelerResources *) shovelerViewGetTarget(view, shovelerComponentViewTargetIdResources);
}
