#ifndef SHOVELER_COMPONENT_RENDER_STATE_H
#define SHOVELER_COMPONENT_RENDER_STATE_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerRenderStateStruct ShovelerRenderState; // forward declaration: render_state.h

extern const char *const shovelerComponentViewTargetIdRenderState;

ShovelerRenderState *shovelerComponentGetViewRenderState(ShovelerComponent *component);

static inline bool shovelerComponentHasViewRenderState(ShovelerComponent *component)
{
	return shovelerComponentGetViewRenderState(component) != NULL;
}

#endif
