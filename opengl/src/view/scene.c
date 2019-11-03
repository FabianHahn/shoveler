#include "shoveler/view/scene.h"

#include "shoveler/component/scene.h"
#include "shoveler/scene.h"
#include "shoveler/view.h"

bool shovelerViewSetScene(ShovelerView *view, ShovelerScene *scene)
{
	return shovelerViewSetTarget(view, shovelerComponentViewTargetIdScene, scene);
}

ShovelerScene *shovelerViewGetScene(ShovelerView *view)
{
	return (ShovelerScene *) shovelerViewGetTarget(view, shovelerComponentViewTargetIdScene);
}
