#include "shoveler/component/scene.h"

#include "shoveler/component.h"
#include "shoveler/scene.h"

const char *const shovelerComponentViewTargetIdScene = "scene";

ShovelerScene *shovelerComponentGetViewScene(ShovelerComponent *component)
{
	return (ShovelerScene *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdScene);
}
