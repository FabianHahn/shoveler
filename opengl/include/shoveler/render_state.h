#ifndef SHOVELER_RENDER_STATE_H
#define SHOVELER_RENDER_STATE_H

#include <stdbool.h> // bool

#include <glad/glad.h>

typedef struct ShovelerRenderStateStruct {
	bool blend;
	GLenum blendSourceFactor;
	GLenum blendDestinationFactor;
	bool depthTest;
	GLenum depthFunction;
	GLboolean depthMask;
} ShovelerRenderState;

void shovelerRenderStateReset(const ShovelerRenderState *renderState);
void shovelerRenderStateSet(ShovelerRenderState *renderState, const ShovelerRenderState *targetRenderState);
/** Same as shovelerRenderStateSet, but adds a log line for every performed change. */
void shovelerRenderStateSetVerbose(ShovelerRenderState *renderState, const ShovelerRenderState *targetRenderState);
void shovelerRenderStateEnableBlend(ShovelerRenderState *renderState, GLenum sourceFactor, GLenum destinationFactor);
void shovelerRenderStateDisableBlend(ShovelerRenderState *renderState);
void shovelerRenderStateEnableDepthTest(ShovelerRenderState *renderState, GLenum depthFunction);
void shovelerRenderStateDisableDepthTest(ShovelerRenderState *renderState);
void shovelerRenderStateSetDepthMask(ShovelerRenderState *renderState, GLboolean enabled);

#endif
