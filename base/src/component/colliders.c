#include "shoveler/component/colliders.h"

#include "shoveler/component.h"
#include "shoveler/colliders.h"

ShovelerColliders *shovelerComponentGetViewColliders(ShovelerComponent *component)
{
	return (ShovelerColliders *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetNameColliders);
}
