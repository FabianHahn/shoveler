#include "shoveler/projection.h"

#include <math.h> // tanf

#include "shoveler/frustum.h"
#include "shoveler/types.h"

static void printVector(const char* name, ShovelerVector3 v);
static void computePlane(
    ShovelerVector3 p0, ShovelerVector3 p1, ShovelerVector3 p2, ShovelerPlane* outputPlane);

void shovelerProjectionPerspectiveComputeTransformation(
    const ShovelerProjectionPerspective* projection, ShovelerMatrix* outputTransformation) {
  *outputTransformation = shovelerMatrixZero;

  float f = 1.0f / tanf(projection->fieldOfViewY / 2.0f);
  shovelerMatrixGet(*outputTransformation, 0, 0) = f / projection->aspectRatio;
  shovelerMatrixGet(*outputTransformation, 1, 1) = f;
  shovelerMatrixGet(*outputTransformation, 2, 2) =
      (projection->nearClippingPlane + projection->farClippingPlane) /
      (projection->nearClippingPlane - projection->farClippingPlane);
  shovelerMatrixGet(*outputTransformation, 2, 3) =
      (2.0f * projection->farClippingPlane * projection->nearClippingPlane) /
      (projection->nearClippingPlane - projection->farClippingPlane);
  shovelerMatrixGet(*outputTransformation, 3, 2) = -1.0f;
}

void shovelerProjectionPerspectiveComputeFrustum(
    const ShovelerProjectionPerspective* projection,
    const ShovelerReferenceFrame* frame,
    ShovelerFrustum* outputFrustum) {
  ShovelerVector3 right = shovelerVector3Cross(frame->direction, frame->up);

  ShovelerVector3 nearCenter = shovelerVector3LinearCombination(
      1.0f, frame->position, projection->nearClippingPlane, frame->direction);
  ShovelerVector3 farCenter = shovelerVector3LinearCombination(
      1.0f, frame->position, projection->farClippingPlane, frame->direction);

  float heightExtentFactor = tanf(0.5f * projection->fieldOfViewY);
  float halfHeightNear = heightExtentFactor * projection->nearClippingPlane;
  float halfHeightFar = heightExtentFactor * projection->farClippingPlane;
  float halfWidthNear = halfHeightNear * projection->aspectRatio;
  float halfWidthFar = halfHeightFar * projection->aspectRatio;

  ShovelerVector3 nearBottomCenter =
      shovelerVector3LinearCombination(1.0, nearCenter, -halfHeightNear, frame->up);
  outputFrustum->nearBottomLeftVertex =
      shovelerVector3LinearCombination(1.0, nearBottomCenter, -halfWidthNear, right);
  outputFrustum->nearBottomRightVertex =
      shovelerVector3LinearCombination(1.0, nearBottomCenter, halfWidthNear, right);

  ShovelerVector3 nearTopCenter =
      shovelerVector3LinearCombination(1.0, nearCenter, halfHeightNear, frame->up);
  outputFrustum->nearTopLeftVertex =
      shovelerVector3LinearCombination(1.0, nearTopCenter, -halfWidthNear, right);
  outputFrustum->nearTopRightVertex =
      shovelerVector3LinearCombination(1.0, nearTopCenter, halfWidthNear, right);

  ShovelerVector3 farBottomCenter =
      shovelerVector3LinearCombination(1.0, farCenter, -halfHeightFar, frame->up);
  outputFrustum->farBottomLeftVertex =
      shovelerVector3LinearCombination(1.0, farBottomCenter, -halfWidthFar, right);
  outputFrustum->farBottomRightVertex =
      shovelerVector3LinearCombination(1.0, farBottomCenter, halfWidthFar, right);

  ShovelerVector3 farTopCenter =
      shovelerVector3LinearCombination(1.0, farCenter, halfHeightFar, frame->up);
  outputFrustum->farTopLeftVertex =
      shovelerVector3LinearCombination(1.0, farTopCenter, -halfWidthFar, right);
  outputFrustum->farTopRightVertex =
      shovelerVector3LinearCombination(1.0, farTopCenter, halfWidthFar, right);

  outputFrustum->nearPlane.normal =
      shovelerVector3LinearCombination(-1.0f, frame->direction, 0.0f, frame->direction);
  outputFrustum->nearPlane.offset = shovelerVector3Dot(outputFrustum->nearPlane.normal, nearCenter);

  outputFrustum->farPlane.normal = frame->direction;
  outputFrustum->farPlane.offset = shovelerVector3Dot(outputFrustum->farPlane.normal, farCenter);

  computePlane(
      outputFrustum->nearBottomLeftVertex,
      outputFrustum->farBottomLeftVertex,
      outputFrustum->farTopLeftVertex,
      &outputFrustum->leftPlane);
  computePlane(
      outputFrustum->nearBottomRightVertex,
      outputFrustum->farBottomRightVertex,
      outputFrustum->farBottomLeftVertex,
      &outputFrustum->bottomPlane);
  computePlane(
      outputFrustum->nearTopRightVertex,
      outputFrustum->farTopRightVertex,
      outputFrustum->farBottomRightVertex,
      &outputFrustum->rightPlane);
  computePlane(
      outputFrustum->nearTopLeftVertex,
      outputFrustum->farTopLeftVertex,
      outputFrustum->farTopRightVertex,
      &outputFrustum->topPlane);
}

static void computePlane(
    ShovelerVector3 p0, ShovelerVector3 p1, ShovelerVector3 p2, ShovelerPlane* outputPlane) {
  ShovelerVector3 direction1 = shovelerVector3LinearCombination(1.0f, p1, -1.0f, p0);
  ShovelerVector3 direction2 = shovelerVector3LinearCombination(1.0f, p2, -1.0f, p1);

  outputPlane->normal = shovelerVector3Normalize(shovelerVector3Cross(direction2, direction1));
  outputPlane->offset = shovelerVector3Dot(outputPlane->normal, p0);
}
