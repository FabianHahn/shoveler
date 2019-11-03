#include "shoveler/view/controller.h"

#include "shoveler/component/controller.h"
#include "shoveler/controller.h"
#include "shoveler/view.h"

bool shovelerViewSetController(ShovelerView *view, ShovelerController *controller)
{
	return shovelerViewSetTarget(view, shovelerComponentViewTargetIdController, controller);
}

ShovelerController *shovelerViewGetController(ShovelerView *view)
{
	return (ShovelerController *) shovelerViewGetTarget(view, shovelerComponentViewTargetIdController);
}
