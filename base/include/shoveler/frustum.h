#ifndef SHOVELER_FRUSTUM_H
#define SHOVELER_FRUSTUM_H

#include <stdbool.h>

#include <shoveler/types.h>

typedef struct {
	ShovelerVector3 nearBottomLeft;
	ShovelerVector3 nearBottomRight;
	ShovelerVector3 nearTopRight;
	ShovelerVector3 nearTopLeft;
	ShovelerVector3 farBottomLeft;
	ShovelerVector3 farBottomRight;
	ShovelerVector3 farTopRight;
	ShovelerVector3 farTopLeft;
	ShovelerPlane near;
	ShovelerPlane far;
	ShovelerPlane left;
	ShovelerPlane bottom;
	ShovelerPlane right;
	ShovelerPlane top;
} ShovelerFrustum;

/** Returns true if the passed frustums intersect. */
bool shovelerFrustumIntersectFrustum(const ShovelerFrustum *frustum, const ShovelerFrustum *otherFrustum);

#endif
