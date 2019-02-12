#ifndef SHOVELER_TYPES_H
#define SHOVELER_TYPES_H

#include <math.h> // sqrtf
#include <stdbool.h> // bool

typedef enum {
	SHOVELER_COORDINATE_MAPPING_POSITIVE_X,
	SHOVELER_COORDINATE_MAPPING_NEGATIVE_X,
	SHOVELER_COORDINATE_MAPPING_POSITIVE_Y,
	SHOVELER_COORDINATE_MAPPING_NEGATIVE_Y,
	SHOVELER_COORDINATE_MAPPING_POSITIVE_Z,
	SHOVELER_COORDINATE_MAPPING_NEGATIVE_Z,
} ShovelerCoordinateMapping;

typedef struct {
	float values[2];
} ShovelerVector2;

typedef struct {
	float values[3];
} ShovelerVector3;

typedef struct {
	float values[4];
} ShovelerVector4;

typedef struct {
	float values[16];
} ShovelerMatrix;

typedef struct {
	ShovelerVector3 normal;
	float offset;
} ShovelerPlane;

typedef struct {
	ShovelerVector3 position;
	ShovelerVector3 direction;
	ShovelerVector3 up;
} ShovelerReferenceFrame;

typedef struct {
	ShovelerVector2 min;
	ShovelerVector2 max;
} ShovelerBoundingBox2;

typedef struct {
	ShovelerVector3 min;
	ShovelerVector3 max;
} ShovelerBoundingBox3;

static inline ShovelerVector2 shovelerVector2(float x, float y) { ShovelerVector2 v = {x, y}; return v; }
static inline ShovelerVector3 shovelerVector3(float x, float y, float z) { ShovelerVector3 v = {x, y, z}; return v; }
static inline ShovelerVector4 shovelerVector4(float x, float y, float z, float w) { ShovelerVector4 v = {x, y, z, w}; return v; }

static inline ShovelerMatrix shovelerMatrix(
	float a11, float a12, float a13, float a14,
	float a21, float a22, float a23, float a24,
	float a31, float a32, float a33, float a34,
	float a41, float a42, float a43, float a44)
{
	ShovelerMatrix m = {
		a11, a12, a13, a14,
		a21, a22, a23, a24,
		a31, a32, a33, a34,
		a41, a42, a43, a44};
	return m;
}

static inline ShovelerPlane shovelerPlane(ShovelerVector3 normal, float offset)
{
	ShovelerPlane plane = {normal, offset};
	return plane;
}

static inline ShovelerReferenceFrame shovelerReferenceFrame(ShovelerVector3 position, ShovelerVector3 direction, ShovelerVector3 up)
{
	ShovelerReferenceFrame frame = {position, direction, up};
	return frame;
}

static inline ShovelerBoundingBox2 shovelerBoundingBox2(ShovelerVector2 min, ShovelerVector2 max)
{
	ShovelerBoundingBox2 box = {min, max};
	return box;
}

static inline ShovelerBoundingBox3 shovelerBoundingBox3(ShovelerVector3 min, ShovelerVector3 max)
{
	ShovelerBoundingBox3 box = {min, max};
	return box;
}

#define shovelerMatrixGet(MATRIX, ROW, COL) (MATRIX).values[(ROW) * 4 + (COL)]
#define shovelerMatrixSet(MATRIX, ROW, COL, VALUE) shovelerMatrixGet(MATRIX, ROW, COL) = VALUE

static ShovelerMatrix shovelerMatrixZero = {
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0};

static ShovelerMatrix shovelerMatrixIdentity = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1};

static inline ShovelerMatrix shovelerMatrixTranspose(ShovelerMatrix A)
{
	ShovelerMatrix AT;
	shovelerMatrixSet(AT, 0, 0, shovelerMatrixGet(A, 0, 0));
	shovelerMatrixSet(AT, 0, 1, shovelerMatrixGet(A, 1, 0));
	shovelerMatrixSet(AT, 0, 2, shovelerMatrixGet(A, 2, 0));
	shovelerMatrixSet(AT, 0, 3, shovelerMatrixGet(A, 3, 0));

	shovelerMatrixSet(AT, 1, 0, shovelerMatrixGet(A, 0, 1));
	shovelerMatrixSet(AT, 1, 1, shovelerMatrixGet(A, 1, 1));
	shovelerMatrixSet(AT, 1, 2, shovelerMatrixGet(A, 2, 1));
	shovelerMatrixSet(AT, 1, 3, shovelerMatrixGet(A, 3, 1));

	shovelerMatrixSet(AT, 2, 0, shovelerMatrixGet(A, 0, 2));
	shovelerMatrixSet(AT, 2, 1, shovelerMatrixGet(A, 1, 2));
	shovelerMatrixSet(AT, 2, 2, shovelerMatrixGet(A, 2, 2));
	shovelerMatrixSet(AT, 2, 3, shovelerMatrixGet(A, 3, 2));

	shovelerMatrixSet(AT, 3, 0, shovelerMatrixGet(A, 0, 3));
	shovelerMatrixSet(AT, 3, 1, shovelerMatrixGet(A, 1, 3));
	shovelerMatrixSet(AT, 3, 2, shovelerMatrixGet(A, 2, 3));
	shovelerMatrixSet(AT, 3, 3, shovelerMatrixGet(A, 3, 3));

	return AT;
}

static inline ShovelerMatrix shovelerMatrixMultiply(ShovelerMatrix A, ShovelerMatrix B)
{
	ShovelerMatrix C;
	shovelerMatrixSet(C, 0, 0,
		shovelerMatrixGet(A, 0, 0) * shovelerMatrixGet(B, 0, 0) +
		shovelerMatrixGet(A, 0, 1) * shovelerMatrixGet(B, 1, 0) +
		shovelerMatrixGet(A, 0, 2) * shovelerMatrixGet(B, 2, 0) +
		shovelerMatrixGet(A, 0, 3) * shovelerMatrixGet(B, 3, 0));
	shovelerMatrixSet(C, 0, 1,
		shovelerMatrixGet(A, 0, 0) * shovelerMatrixGet(B, 0, 1) +
		shovelerMatrixGet(A, 0, 1) * shovelerMatrixGet(B, 1, 1) +
		shovelerMatrixGet(A, 0, 2) * shovelerMatrixGet(B, 2, 1) +
		shovelerMatrixGet(A, 0, 3) * shovelerMatrixGet(B, 3, 1));
	shovelerMatrixSet(C, 0, 2,
		shovelerMatrixGet(A, 0, 0) * shovelerMatrixGet(B, 0, 2) +
		shovelerMatrixGet(A, 0, 1) * shovelerMatrixGet(B, 1, 2) +
		shovelerMatrixGet(A, 0, 2) * shovelerMatrixGet(B, 2, 2) +
		shovelerMatrixGet(A, 0, 3) * shovelerMatrixGet(B, 3, 2));
	shovelerMatrixSet(C, 0, 3,
		shovelerMatrixGet(A, 0, 0) * shovelerMatrixGet(B, 0, 3) +
		shovelerMatrixGet(A, 0, 1) * shovelerMatrixGet(B, 1, 3) +
		shovelerMatrixGet(A, 0, 2) * shovelerMatrixGet(B, 2, 3) +
		shovelerMatrixGet(A, 0, 3) * shovelerMatrixGet(B, 3, 3));

	shovelerMatrixSet(C, 1, 0,
		shovelerMatrixGet(A, 1, 0) * shovelerMatrixGet(B, 0, 0) +
		shovelerMatrixGet(A, 1, 1) * shovelerMatrixGet(B, 1, 0) +
		shovelerMatrixGet(A, 1, 2) * shovelerMatrixGet(B, 2, 0) +
		shovelerMatrixGet(A, 1, 3) * shovelerMatrixGet(B, 3, 0));
	shovelerMatrixSet(C, 1, 1,
		shovelerMatrixGet(A, 1, 0) * shovelerMatrixGet(B, 0, 1) +
		shovelerMatrixGet(A, 1, 1) * shovelerMatrixGet(B, 1, 1) +
		shovelerMatrixGet(A, 1, 2) * shovelerMatrixGet(B, 2, 1) +
		shovelerMatrixGet(A, 1, 3) * shovelerMatrixGet(B, 3, 1));
	shovelerMatrixSet(C, 1, 2,
		shovelerMatrixGet(A, 1, 0) * shovelerMatrixGet(B, 0, 2) +
		shovelerMatrixGet(A, 1, 1) * shovelerMatrixGet(B, 1, 2) +
		shovelerMatrixGet(A, 1, 2) * shovelerMatrixGet(B, 2, 2) +
		shovelerMatrixGet(A, 1, 3) * shovelerMatrixGet(B, 3, 2));
	shovelerMatrixSet(C, 1, 3,
		shovelerMatrixGet(A, 1, 0) * shovelerMatrixGet(B, 0, 3) +
		shovelerMatrixGet(A, 1, 1) * shovelerMatrixGet(B, 1, 3) +
		shovelerMatrixGet(A, 1, 2) * shovelerMatrixGet(B, 2, 3) +
		shovelerMatrixGet(A, 1, 3) * shovelerMatrixGet(B, 3, 3));

	shovelerMatrixSet(C, 2, 0,
		shovelerMatrixGet(A, 2, 0) * shovelerMatrixGet(B, 0, 0) +
		shovelerMatrixGet(A, 2, 1) * shovelerMatrixGet(B, 1, 0) +
		shovelerMatrixGet(A, 2, 2) * shovelerMatrixGet(B, 2, 0) +
		shovelerMatrixGet(A, 2, 3) * shovelerMatrixGet(B, 3, 0));
	shovelerMatrixSet(C, 2, 1,
		shovelerMatrixGet(A, 2, 0) * shovelerMatrixGet(B, 0, 1) +
		shovelerMatrixGet(A, 2, 1) * shovelerMatrixGet(B, 1, 1) +
		shovelerMatrixGet(A, 2, 2) * shovelerMatrixGet(B, 2, 1) +
		shovelerMatrixGet(A, 2, 3) * shovelerMatrixGet(B, 3, 1));
	shovelerMatrixSet(C, 2, 2,
		shovelerMatrixGet(A, 2, 0) * shovelerMatrixGet(B, 0, 2) +
		shovelerMatrixGet(A, 2, 1) * shovelerMatrixGet(B, 1, 2) +
		shovelerMatrixGet(A, 2, 2) * shovelerMatrixGet(B, 2, 2) +
		shovelerMatrixGet(A, 2, 3) * shovelerMatrixGet(B, 3, 2));
	shovelerMatrixSet(C, 2, 3,
		shovelerMatrixGet(A, 2, 0) * shovelerMatrixGet(B, 0, 3) +
		shovelerMatrixGet(A, 2, 1) * shovelerMatrixGet(B, 1, 3) +
		shovelerMatrixGet(A, 2, 2) * shovelerMatrixGet(B, 2, 3) +
		shovelerMatrixGet(A, 2, 3) * shovelerMatrixGet(B, 3, 3));

	shovelerMatrixSet(C, 3, 0,
		shovelerMatrixGet(A, 3, 0) * shovelerMatrixGet(B, 0, 0) +
		shovelerMatrixGet(A, 3, 1) * shovelerMatrixGet(B, 1, 0) +
		shovelerMatrixGet(A, 3, 2) * shovelerMatrixGet(B, 2, 0) +
		shovelerMatrixGet(A, 3, 3) * shovelerMatrixGet(B, 3, 0));
	shovelerMatrixSet(C, 3, 1,
		shovelerMatrixGet(A, 3, 0) * shovelerMatrixGet(B, 0, 1) +
		shovelerMatrixGet(A, 3, 1) * shovelerMatrixGet(B, 1, 1) +
		shovelerMatrixGet(A, 3, 2) * shovelerMatrixGet(B, 2, 1) +
		shovelerMatrixGet(A, 3, 3) * shovelerMatrixGet(B, 3, 1));
	shovelerMatrixSet(C, 3, 2,
		shovelerMatrixGet(A, 3, 0) * shovelerMatrixGet(B, 0, 2) +
		shovelerMatrixGet(A, 3, 1) * shovelerMatrixGet(B, 1, 2) +
		shovelerMatrixGet(A, 3, 2) * shovelerMatrixGet(B, 2, 2) +
		shovelerMatrixGet(A, 3, 3) * shovelerMatrixGet(B, 3, 2));
	shovelerMatrixSet(C, 3, 3,
		shovelerMatrixGet(A, 3, 0) * shovelerMatrixGet(B, 0, 3) +
		shovelerMatrixGet(A, 3, 1) * shovelerMatrixGet(B, 1, 3) +
		shovelerMatrixGet(A, 3, 2) * shovelerMatrixGet(B, 2, 3) +
		shovelerMatrixGet(A, 3, 3) * shovelerMatrixGet(B, 3, 3));

	return C;
}

static inline ShovelerVector4 shovelerMatrixMultiplyVector4(ShovelerMatrix A, ShovelerVector4 b)
{
	ShovelerVector4 c;
	c.values[0] = shovelerMatrixGet(A, 0, 0) * b.values[0] + shovelerMatrixGet(A, 0, 1) * b.values[1] + shovelerMatrixGet(A, 0, 2) * b.values[2] + shovelerMatrixGet(A, 0, 3) * b.values[3];
	c.values[1] = shovelerMatrixGet(A, 1, 0) * b.values[0] + shovelerMatrixGet(A, 1, 1) * b.values[1] + shovelerMatrixGet(A, 1, 2) * b.values[2] + shovelerMatrixGet(A, 1, 3) * b.values[3];
	c.values[2] = shovelerMatrixGet(A, 2, 0) * b.values[0] + shovelerMatrixGet(A, 2, 1) * b.values[1] + shovelerMatrixGet(A, 2, 2) * b.values[2] + shovelerMatrixGet(A, 2, 3) * b.values[3];
	c.values[3] = shovelerMatrixGet(A, 3, 0) * b.values[0] + shovelerMatrixGet(A, 3, 1) * b.values[1] + shovelerMatrixGet(A, 3, 2) * b.values[2] + shovelerMatrixGet(A, 3, 3) * b.values[3];
	return c;
}

static inline ShovelerVector3 shovelerMatrixMultiplyVector3(ShovelerMatrix A, ShovelerVector3 b)
{
	ShovelerVector4 b4 = shovelerVector4(b.values[0], b.values[1], b.values[2], 1.0f);
	ShovelerVector4 c4 = shovelerMatrixMultiplyVector4(A, b4);

	return shovelerVector3(c4.values[0] / c4.values[3], c4.values[1] / c4.values[3], c4.values[2] / c4.values[3]);
}

static inline ShovelerVector2 shovelerMatrixMultiplyVector2(ShovelerMatrix A, ShovelerVector2 b)
{
	ShovelerVector3 b3 = shovelerVector3(b.values[0], b.values[1], 1.0f);
	ShovelerVector3 c3 = shovelerMatrixMultiplyVector3(A, b3);

	return shovelerVector2(c3.values[0] / c3.values[2], c3.values[1] / c3.values[2]);
}

static inline ShovelerVector3 shovelerVector3Cross(ShovelerVector3 a, ShovelerVector3 b)
{
	ShovelerVector3 c;
	c.values[0] = a.values[1] * b.values[2] - a.values[2] * b.values[1];
	c.values[1] = a.values[2] * b.values[0] - a.values[0] * b.values[2];
	c.values[2] = a.values[0] * b.values[1] - a.values[1] * b.values[0];
	return c;
}

static inline float shovelerVector3Dot(ShovelerVector3 a, ShovelerVector3 b)
{
	return a.values[0] * b.values[0] + a.values[1] * b.values[1] + a.values[2] * b.values[2];
}

static inline ShovelerVector3 shovelerVector3Normalize(ShovelerVector3 a)
{
	float length = sqrtf(a.values[0] * a.values[0] + a.values[1] * a.values[1] + a.values[2] * a.values[2]);
	ShovelerVector3 an;
	an.values[0] = a.values[0] / length;
	an.values[1] = a.values[1] / length;
	an.values[2] = a.values[2] / length;
	return an;
}

static inline ShovelerVector2 shovelerVector2LinearCombination(float alpha, ShovelerVector2 a, float beta, ShovelerVector2 b)
{
	ShovelerVector2 c;
	c.values[0] = alpha * a.values[0] + beta * b.values[0];
	c.values[1] = alpha * a.values[1] + beta * b.values[1];
	return c;
}

static inline ShovelerVector3 shovelerVector3LinearCombination(float alpha, ShovelerVector3 a, float beta, ShovelerVector3 b)
{
	ShovelerVector3 c;
	c.values[0] = alpha * a.values[0] + beta * b.values[0];
	c.values[1] = alpha * a.values[1] + beta * b.values[1];
	c.values[2] = alpha * a.values[2] + beta * b.values[2];
	return c;
}

static inline ShovelerMatrix shovelerMatrixCreateRotationX(float angle)
{
	ShovelerMatrix rotation = shovelerMatrixIdentity;

	float c = cosf(angle);
	float s = sinf(angle);

	shovelerMatrixGet(rotation, 1, 1) = c;
	shovelerMatrixGet(rotation, 1, 2) = s;
	shovelerMatrixGet(rotation, 2, 1) = -s;
	shovelerMatrixGet(rotation, 2, 2) = c;

	return rotation;
}

static inline ShovelerMatrix shovelerMatrixCreateRotationY(float angle)
{
	ShovelerMatrix rotation = shovelerMatrixIdentity;

	float c = cosf(angle);
	float s = sinf(angle);

	shovelerMatrixGet(rotation, 0, 0) = c;
	shovelerMatrixGet(rotation, 0, 2) = -s;
	shovelerMatrixGet(rotation, 2, 0) = s;
	shovelerMatrixGet(rotation, 2, 2) = c;

	return rotation;
}

static inline ShovelerMatrix shovelerMatrixCreateRotationZ(float angle)
{
	ShovelerMatrix rotation = shovelerMatrixIdentity;

	float c = cosf(angle);
	float s = sinf(angle);

	shovelerMatrixGet(rotation, 0, 0) = c;
	shovelerMatrixGet(rotation, 0, 1) = s;
	shovelerMatrixGet(rotation, 1, 0) = -s;
	shovelerMatrixGet(rotation, 1, 1) = c;

	return rotation;
}

static inline ShovelerMatrix shovelerMatrixCreateRotation(ShovelerVector3 axis, float angle)
{
	ShovelerVector3 axisNormalized = shovelerVector3Normalize(axis);

	float c = cosf(angle);
	float s = sinf(angle);
	float C = 1 - c;
	float x = axisNormalized.values[0];
	float y = axisNormalized.values[1];
	float z = axisNormalized.values[2];
	float xs = x * s;
	float ys = y * s;
	float zs = z * s;
	float xC = x * C;
	float yC = y * C;
	float zC = z * C;
	float xyC = x * yC;
	float yzC = y * zC;
	float zxC = z * xC;

	ShovelerMatrix rotation = shovelerMatrixIdentity;
	shovelerMatrixGet(rotation, 0, 1) = xyC - zs;
	shovelerMatrixGet(rotation, 0, 0) = x * xC + c;
	shovelerMatrixGet(rotation, 0, 2) = zxC + ys;
	shovelerMatrixGet(rotation, 1, 0) = xyC + zs;
	shovelerMatrixGet(rotation, 1, 1) = y * yC + c;
	shovelerMatrixGet(rotation, 1, 2) = yzC - xs;
	shovelerMatrixGet(rotation, 2, 0) = zxC - ys;
	shovelerMatrixGet(rotation, 2, 1) = yzC + xs;
	shovelerMatrixGet(rotation, 2, 2) = z * zC + c;

	return rotation;
}

static inline void shovelerMatrixCreateLookIntoDirectionTransformation(const ShovelerReferenceFrame *frame, ShovelerMatrix *outputTransformation)
{
	// Construct camera coordinate system basis
	ShovelerVector3 right = shovelerVector3Cross(frame->direction, frame->up);

	// Construct basis transform to camera coordinates
	ShovelerMatrix basis = shovelerMatrixIdentity;
	shovelerMatrixGet(basis, 0, 0) = right.values[0];
	shovelerMatrixGet(basis, 0, 1) = right.values[1];
	shovelerMatrixGet(basis, 0, 2) = right.values[2];
	shovelerMatrixGet(basis, 1, 0) = frame->up.values[0];
	shovelerMatrixGet(basis, 1, 1) = frame->up.values[1];
	shovelerMatrixGet(basis, 1, 2) = frame->up.values[2];
	shovelerMatrixGet(basis, 2, 0) = -frame->direction.values[0];
	shovelerMatrixGet(basis, 2, 1) = -frame->direction.values[1];
	shovelerMatrixGet(basis, 2, 2) = -frame->direction.values[2];

	// Construct shift matrix to camera position
	ShovelerMatrix shift = shovelerMatrixIdentity;
	shovelerMatrixGet(shift, 0, 3) = -frame->position.values[0];
	shovelerMatrixGet(shift, 1, 3) = -frame->position.values[1];
	shovelerMatrixGet(shift, 2, 3) = -frame->position.values[2];

	// Create look into direction matrix
	*outputTransformation = shovelerMatrixMultiply(basis, shift);
}

static inline float shovelerPlaneVectorDistance(ShovelerPlane plane, ShovelerVector3 point)
{
	ShovelerVector3 planeOriginToPoint = shovelerVector3LinearCombination(1.0f, point, -plane.offset, plane.normal);
	return shovelerVector3Dot(planeOriginToPoint, plane.normal);
}

static inline bool shovelerBoundingBox2Intersect(const ShovelerBoundingBox2 *first, const ShovelerBoundingBox2 *second)
{
	if(first->max.values[0] <= second->min.values[0]) {
		return false;
	}

	if(first->max.values[1] <= second->min.values[1]) {
		return false;
	}

	if(first->min.values[0] >= second->max.values[0]) {
		return false;
	}

	if(first->min.values[1] >= second->max.values[1]) {
		return false;
	}

	return true;
}

static inline bool shovelerBoundingBox3Intersect(const ShovelerBoundingBox3 *first, const ShovelerBoundingBox3 *second)
{
	if(first->max.values[0] <= second->min.values[0]) {
		return false;
	}

	if(first->max.values[1] <= second->min.values[1]) {
		return false;
	}

	if(first->max.values[2] <= second->min.values[2]) {
		return false;
	}

	if(first->min.values[0] >= second->max.values[0]) {
		return false;
	}

	if(first->min.values[1] >= second->max.values[1]) {
		return false;
	}

	if(first->min.values[2] >= second->max.values[2]) {
		return false;
	}

	return true;
}

static inline float shovelerCoordinateMap(ShovelerVector3 coordinates, ShovelerCoordinateMapping mapping)
{
	switch(mapping) {
		case SHOVELER_COORDINATE_MAPPING_POSITIVE_X:
			return coordinates.values[0];
		case SHOVELER_COORDINATE_MAPPING_NEGATIVE_X:
			return -coordinates.values[0];
		case SHOVELER_COORDINATE_MAPPING_POSITIVE_Y:
			return coordinates.values[1];
		case SHOVELER_COORDINATE_MAPPING_NEGATIVE_Y:
			return -coordinates.values[1];
		case SHOVELER_COORDINATE_MAPPING_POSITIVE_Z:
			return coordinates.values[2];
		case SHOVELER_COORDINATE_MAPPING_NEGATIVE_Z:
			return -coordinates.values[2];
	}
}

#endif
