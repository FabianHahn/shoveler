#include "shoveler/component/scene.h"

#include "shoveler/component.h"
#include "shoveler/scene.h"

ShovelerScene *shovelerComponentGetViewScene(ShovelerComponent *component)
{
	return (ShovelerScene *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetNameScene);
}
