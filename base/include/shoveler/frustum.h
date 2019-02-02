#ifndef SHOVELER_FRUSTUM_H
#define SHOVELER_FRUSTUM_H

#include <stdbool.h>

#include <shoveler/types.h>

typedef struct {
	ShovelerVector3 nearBottomLeftVertex;
	ShovelerVector3 nearBottomRightVertex;
	ShovelerVector3 nearTopRightVertex;
	ShovelerVector3 nearTopLeftVertex;
	ShovelerVector3 farBottomLeftVertex;
	ShovelerVector3 farBottomRightVertex;
	ShovelerVector3 farTopRightVertex;
	ShovelerVector3 farTopLeftVertex;
	ShovelerPlane nearPlane;
	ShovelerPlane farPlane;
	ShovelerPlane leftPlane;
	ShovelerPlane bottomPlane;
	ShovelerPlane rightPlane;
	ShovelerPlane topPlane;
} ShovelerFrustum;

/** Returns true if the passed frustums intersect. */
bool shovelerFrustumIntersectFrustum(const ShovelerFrustum *frustum, const ShovelerFrustum *otherFrustum);

#endif
