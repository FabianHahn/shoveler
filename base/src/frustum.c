#include "shoveler/frustum.h"

#include "shoveler/types.h"

static const float eps = 1e-6f;

bool shovelerFrustumIntersectFrustum(
    const ShovelerFrustum* frustum, const ShovelerFrustum* otherFrustum) {
  ShovelerPlane frustumPlanes[] = {
      frustum->nearPlane,
      frustum->farPlane,
      frustum->leftPlane,
      frustum->bottomPlane,
      frustum->rightPlane,
      frustum->topPlane,
  };
  ShovelerPlane otherFrustumPlanes[] = {
      otherFrustum->nearPlane,
      otherFrustum->farPlane,
      otherFrustum->leftPlane,
      otherFrustum->bottomPlane,
      otherFrustum->rightPlane,
      otherFrustum->topPlane,
  };

  ShovelerVector3 frustumVertices[] = {
      frustum->nearBottomLeftVertex,
      frustum->nearBottomRightVertex,
      frustum->nearTopRightVertex,
      frustum->nearTopLeftVertex,
      frustum->farBottomLeftVertex,
      frustum->farBottomRightVertex,
      frustum->farTopRightVertex,
      frustum->farTopLeftVertex,
  };
  ShovelerVector3 otherFrustumVertices[] = {
      otherFrustum->nearBottomLeftVertex,
      otherFrustum->nearBottomRightVertex,
      otherFrustum->nearTopRightVertex,
      otherFrustum->nearTopLeftVertex,
      otherFrustum->farBottomLeftVertex,
      otherFrustum->farBottomRightVertex,
      otherFrustum->farTopRightVertex,
      otherFrustum->farTopLeftVertex,
  };

  // Test if vertices of other frustum lie within frustum
  for (int i = 0; i < 6; i++) {
    ShovelerPlane frustumPlane = frustumPlanes[i];

    int numVerticesOutsideFrustum = 0;
    for (int j = 0; j < 8; j++) {
      ShovelerVector3 otherFrustumVertex = otherFrustumVertices[j];

      if (shovelerPlaneVectorDistance(frustumPlane, otherFrustumVertex) > eps) {
        numVerticesOutsideFrustum++;
      }
    }

    // Abort if we detect no intersection, since all 8 vertices of the other frustum lie outside
    // this one's plane
    if (numVerticesOutsideFrustum == 8) {
      return false;
    }
  }

  // Test if vertices of frustum lie within other frustum
  for (int i = 0; i < 6; i++) {
    ShovelerPlane otherFrustumPlane = otherFrustumPlanes[i];

    int numVerticesOutsideOtherFrustum = 0;
    for (int j = 0; j < 8; j++) {
      ShovelerVector3 frustumVertex = frustumVertices[j];

      if (shovelerPlaneVectorDistance(otherFrustumPlane, frustumVertex) > eps) {
        numVerticesOutsideOtherFrustum++;
      }
    }

    // Abort if we detect no intersection, since all 8 vertices of the frustum lie outside the plane
    // of the other one
    if (numVerticesOutsideOtherFrustum == 8) {
      return false;
    }
  }

  return true;
}
