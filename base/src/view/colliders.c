#include "shoveler/view/colliders.h"

#include "shoveler/component/colliders.h"
#include "shoveler/colliders.h"
#include "shoveler/view.h"

bool shovelerViewSetColliders(ShovelerView *view, ShovelerColliders *colliders)
{
	return shovelerViewSetTarget(view, shovelerComponentViewTargetIdColliders, colliders);
}

ShovelerColliders *shovelerViewGetColliders(ShovelerView *view)
{
	return (ShovelerColliders *) shovelerViewGetTarget(view, shovelerComponentViewTargetIdColliders);
}
