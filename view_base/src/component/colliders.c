#include "shoveler/component/colliders.h"

#include "shoveler/component.h"
#include "shoveler/colliders.h"

const char *const shovelerComponentViewTargetIdColliders = "colliders";

ShovelerColliders *shovelerComponentGetViewColliders(ShovelerComponent *component)
{
	return (ShovelerColliders *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdColliders);
}
