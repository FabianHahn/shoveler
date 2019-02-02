#include "shoveler/frustum.h"
#include "shoveler/types.h"

static const float eps = 1e-6f;

bool shovelerFrustumIntersectFrustum(const ShovelerFrustum *frustum, const ShovelerFrustum *otherFrustum)
{
	ShovelerPlane frustumPlanes[] = {
		frustum->near,
		frustum->far,
		frustum->left,
		frustum->bottom,
		frustum->right,
		frustum->top,
	};
	ShovelerPlane otherFrustumPlanes[] = {
		otherFrustum->near,
		otherFrustum->far,
		otherFrustum->left,
		otherFrustum->bottom,
		otherFrustum->right,
		otherFrustum->top,
	};

	ShovelerVector3 frustumVertices[] = {
		frustum->nearBottomLeft,
		frustum->nearBottomRight,
		frustum->nearTopRight,
		frustum->nearTopLeft,
		frustum->farBottomLeft,
		frustum->farBottomRight,
		frustum->farTopRight,
		frustum->farTopLeft,
	};
	ShovelerVector3 otherFrustumVertices[] = {
		otherFrustum->nearBottomLeft,
		otherFrustum->nearBottomRight,
		otherFrustum->nearTopRight,
		otherFrustum->nearTopLeft,
		otherFrustum->farBottomLeft,
		otherFrustum->farBottomRight,
		otherFrustum->farTopRight,
		otherFrustum->farTopLeft,
	};

	// Test if vertices of other frustum lie within frustum
	for(int i = 0; i < 6; i++) {
		ShovelerPlane frustumPlane = frustumPlanes[i];

		int numVerticesOutsideFrustum = 0;
		for(int j = 0; j < 8; j++) {
			ShovelerVector3 otherFrustumVertex = otherFrustumVertices[j];

			if(shovelerPlaneVectorDistance(frustumPlane, otherFrustumVertex) > eps) {
				numVerticesOutsideFrustum++;
			}
		}

		// Abort if we detect no intersection, since all 8 vertices of the other frustum lie outside this one's plane
		if(numVerticesOutsideFrustum == 8) {
			return false;
		}
	}

	// Test if vertices of frustum lie within other frustum
	for(int i = 0; i < 6; i++) {
		ShovelerPlane otherFrustumPlane = otherFrustumPlanes[i];

		int numVerticesOutsideOtherFrustum = 0;
		for(int j = 0; j < 8; j++) {
			ShovelerVector3 frustumVertex = frustumVertices[j];

			if(shovelerPlaneVectorDistance(otherFrustumPlane, frustumVertex) > eps) {
				numVerticesOutsideOtherFrustum++;
			}
		}

		// Abort if we detect no intersection, since all 8 vertices of the frustum lie outside the plane of the other one
		if(numVerticesOutsideOtherFrustum == 8) {
			return false;
		}

	}

	return true;
}
