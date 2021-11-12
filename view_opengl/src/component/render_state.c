#include "shoveler/component/render_state.h"

#include "shoveler/component.h"

const char *const shovelerComponentViewTargetIdRenderState = "render_state";

ShovelerRenderState *shovelerComponentGetViewRenderState(ShovelerComponent *component)
{
	return (ShovelerRenderState *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdRenderState);
}
