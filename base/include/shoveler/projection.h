#ifndef SHOVELER_PROJECTION_H
#define SHOVELER_PROJECTION_H

#include <shoveler/frustum.h>
#include <shoveler/types.h>

typedef struct {
  float fieldOfViewY;
  float aspectRatio;
  float nearClippingPlane;
  float farClippingPlane;
} ShovelerProjectionPerspective;

/** Computes the perspective transformation matrix for a perspective camera configuration. */
void shovelerProjectionPerspectiveComputeTransformation(
    const ShovelerProjectionPerspective* projection, ShovelerMatrix* outputTransformation);
/** Computes the frustum of a perspetive projection as seen from a given reference frame. */
void shovelerProjectionPerspectiveComputeFrustum(
    const ShovelerProjectionPerspective* projection,
    const ShovelerReferenceFrame* frame,
    ShovelerFrustum* outputFrustum);

static inline ShovelerProjectionPerspective shovelerProjectionPerspective(
    float fieldOfViewY, float aspectRatio, float nearClippingPlane, float farClippingPlane) {
  ShovelerProjectionPerspective projection;
  projection.fieldOfViewY = fieldOfViewY;
  projection.aspectRatio = aspectRatio;
  projection.nearClippingPlane = nearClippingPlane;
  projection.farClippingPlane = farClippingPlane;
  return projection;
}

#endif
