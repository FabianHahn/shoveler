#ifndef SHOVELER_TYPES_H
#define SHOVELER_TYPES_H

#include <math.h> // sqrtf

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

#define ShovelerMatrixGet(MATRIX, ROW, COL) MATRIX.values[ROW * 4 + COL]
#define ShovelerMatrixSet(MATRIX, ROW, COL, VALUE) ShovelerMatrixGet(MATRIX, ROW, COL) = VALUE

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
	ShovelerMatrixSet(AT, 0, 0, ShovelerMatrixGet(A, 0, 0));
	ShovelerMatrixSet(AT, 0, 1, ShovelerMatrixGet(A, 1, 0));
	ShovelerMatrixSet(AT, 0, 2, ShovelerMatrixGet(A, 2, 0));
	ShovelerMatrixSet(AT, 0, 3, ShovelerMatrixGet(A, 3, 0));

	ShovelerMatrixSet(AT, 1, 0, ShovelerMatrixGet(A, 0, 1));
	ShovelerMatrixSet(AT, 1, 1, ShovelerMatrixGet(A, 1, 1));
	ShovelerMatrixSet(AT, 1, 2, ShovelerMatrixGet(A, 2, 1));
	ShovelerMatrixSet(AT, 1, 3, ShovelerMatrixGet(A, 3, 1));

	ShovelerMatrixSet(AT, 2, 0, ShovelerMatrixGet(A, 0, 2));
	ShovelerMatrixSet(AT, 2, 1, ShovelerMatrixGet(A, 1, 2));
	ShovelerMatrixSet(AT, 2, 2, ShovelerMatrixGet(A, 2, 2));
	ShovelerMatrixSet(AT, 2, 3, ShovelerMatrixGet(A, 3, 2));

	ShovelerMatrixSet(AT, 3, 0, ShovelerMatrixGet(A, 0, 3));
	ShovelerMatrixSet(AT, 3, 1, ShovelerMatrixGet(A, 1, 3));
	ShovelerMatrixSet(AT, 3, 2, ShovelerMatrixGet(A, 2, 3));
	ShovelerMatrixSet(AT, 3, 3, ShovelerMatrixGet(A, 3, 3));

	return AT;
}

static inline ShovelerMatrix shovelerMatrixMultiply(ShovelerMatrix A, ShovelerMatrix B)
{
	ShovelerMatrix C;
	ShovelerMatrixSet(C, 0, 0,
		ShovelerMatrixGet(A, 0, 0) * ShovelerMatrixGet(B, 0, 0) +
		ShovelerMatrixGet(A, 0, 1) * ShovelerMatrixGet(B, 1, 0) +
		ShovelerMatrixGet(A, 0, 2) * ShovelerMatrixGet(B, 2, 0) +
		ShovelerMatrixGet(A, 0, 3) * ShovelerMatrixGet(B, 3, 0));
	ShovelerMatrixSet(C, 0, 1,
		ShovelerMatrixGet(A, 0, 0) * ShovelerMatrixGet(B, 0, 1) +
		ShovelerMatrixGet(A, 0, 1) * ShovelerMatrixGet(B, 1, 1) +
		ShovelerMatrixGet(A, 0, 2) * ShovelerMatrixGet(B, 2, 1) +
		ShovelerMatrixGet(A, 0, 3) * ShovelerMatrixGet(B, 3, 1));
	ShovelerMatrixSet(C, 0, 2,
		ShovelerMatrixGet(A, 0, 0) * ShovelerMatrixGet(B, 0, 2) +
		ShovelerMatrixGet(A, 0, 1) * ShovelerMatrixGet(B, 1, 2) +
		ShovelerMatrixGet(A, 0, 2) * ShovelerMatrixGet(B, 2, 2) +
		ShovelerMatrixGet(A, 0, 3) * ShovelerMatrixGet(B, 3, 2));
	ShovelerMatrixSet(C, 0, 3,
		ShovelerMatrixGet(A, 0, 0) * ShovelerMatrixGet(B, 0, 3) +
		ShovelerMatrixGet(A, 0, 1) * ShovelerMatrixGet(B, 1, 3) +
		ShovelerMatrixGet(A, 0, 2) * ShovelerMatrixGet(B, 2, 3) +
		ShovelerMatrixGet(A, 0, 3) * ShovelerMatrixGet(B, 3, 3));

	ShovelerMatrixSet(C, 1, 0,
		ShovelerMatrixGet(A, 1, 0) * ShovelerMatrixGet(B, 0, 0) +
		ShovelerMatrixGet(A, 1, 1) * ShovelerMatrixGet(B, 1, 0) +
		ShovelerMatrixGet(A, 1, 2) * ShovelerMatrixGet(B, 2, 0) +
		ShovelerMatrixGet(A, 1, 3) * ShovelerMatrixGet(B, 3, 0));
	ShovelerMatrixSet(C, 1, 1,
		ShovelerMatrixGet(A, 1, 0) * ShovelerMatrixGet(B, 0, 1) +
		ShovelerMatrixGet(A, 1, 1) * ShovelerMatrixGet(B, 1, 1) +
		ShovelerMatrixGet(A, 1, 2) * ShovelerMatrixGet(B, 2, 1) +
		ShovelerMatrixGet(A, 1, 3) * ShovelerMatrixGet(B, 3, 1));
	ShovelerMatrixSet(C, 1, 2,
		ShovelerMatrixGet(A, 1, 0) * ShovelerMatrixGet(B, 0, 2) +
		ShovelerMatrixGet(A, 1, 1) * ShovelerMatrixGet(B, 1, 2) +
		ShovelerMatrixGet(A, 1, 2) * ShovelerMatrixGet(B, 2, 2) +
		ShovelerMatrixGet(A, 1, 3) * ShovelerMatrixGet(B, 3, 2));
	ShovelerMatrixSet(C, 1, 3,
		ShovelerMatrixGet(A, 1, 0) * ShovelerMatrixGet(B, 0, 3) +
		ShovelerMatrixGet(A, 1, 1) * ShovelerMatrixGet(B, 1, 3) +
		ShovelerMatrixGet(A, 1, 2) * ShovelerMatrixGet(B, 2, 3) +
		ShovelerMatrixGet(A, 1, 3) * ShovelerMatrixGet(B, 3, 3));

	ShovelerMatrixSet(C, 2, 0,
		ShovelerMatrixGet(A, 2, 0) * ShovelerMatrixGet(B, 0, 0) +
		ShovelerMatrixGet(A, 2, 1) * ShovelerMatrixGet(B, 1, 0) +
		ShovelerMatrixGet(A, 2, 2) * ShovelerMatrixGet(B, 2, 0) +
		ShovelerMatrixGet(A, 2, 3) * ShovelerMatrixGet(B, 3, 0));
	ShovelerMatrixSet(C, 2, 1,
		ShovelerMatrixGet(A, 2, 0) * ShovelerMatrixGet(B, 0, 1) +
		ShovelerMatrixGet(A, 2, 1) * ShovelerMatrixGet(B, 1, 1) +
		ShovelerMatrixGet(A, 2, 2) * ShovelerMatrixGet(B, 2, 1) +
		ShovelerMatrixGet(A, 2, 3) * ShovelerMatrixGet(B, 3, 1));
	ShovelerMatrixSet(C, 2, 2,
		ShovelerMatrixGet(A, 2, 0) * ShovelerMatrixGet(B, 0, 2) +
		ShovelerMatrixGet(A, 2, 1) * ShovelerMatrixGet(B, 1, 2) +
		ShovelerMatrixGet(A, 2, 2) * ShovelerMatrixGet(B, 2, 2) +
		ShovelerMatrixGet(A, 2, 3) * ShovelerMatrixGet(B, 3, 2));
	ShovelerMatrixSet(C, 2, 3,
		ShovelerMatrixGet(A, 2, 0) * ShovelerMatrixGet(B, 0, 3) +
		ShovelerMatrixGet(A, 2, 1) * ShovelerMatrixGet(B, 1, 3) +
		ShovelerMatrixGet(A, 2, 2) * ShovelerMatrixGet(B, 2, 3) +
		ShovelerMatrixGet(A, 2, 3) * ShovelerMatrixGet(B, 3, 3));

	ShovelerMatrixSet(C, 3, 0,
		ShovelerMatrixGet(A, 3, 0) * ShovelerMatrixGet(B, 0, 0) +
		ShovelerMatrixGet(A, 3, 1) * ShovelerMatrixGet(B, 1, 0) +
		ShovelerMatrixGet(A, 3, 2) * ShovelerMatrixGet(B, 2, 0) +
		ShovelerMatrixGet(A, 3, 3) * ShovelerMatrixGet(B, 3, 0));
	ShovelerMatrixSet(C, 3, 1,
		ShovelerMatrixGet(A, 3, 0) * ShovelerMatrixGet(B, 0, 1) +
		ShovelerMatrixGet(A, 3, 1) * ShovelerMatrixGet(B, 1, 1) +
		ShovelerMatrixGet(A, 3, 2) * ShovelerMatrixGet(B, 2, 1) +
		ShovelerMatrixGet(A, 3, 3) * ShovelerMatrixGet(B, 3, 1));
	ShovelerMatrixSet(C, 3, 2,
		ShovelerMatrixGet(A, 3, 0) * ShovelerMatrixGet(B, 0, 2) +
		ShovelerMatrixGet(A, 3, 1) * ShovelerMatrixGet(B, 1, 2) +
		ShovelerMatrixGet(A, 3, 2) * ShovelerMatrixGet(B, 2, 2) +
		ShovelerMatrixGet(A, 3, 3) * ShovelerMatrixGet(B, 3, 2));
	ShovelerMatrixSet(C, 3, 3,
		ShovelerMatrixGet(A, 3, 0) * ShovelerMatrixGet(B, 0, 3) +
		ShovelerMatrixGet(A, 3, 1) * ShovelerMatrixGet(B, 1, 3) +
		ShovelerMatrixGet(A, 3, 2) * ShovelerMatrixGet(B, 2, 3) +
		ShovelerMatrixGet(A, 3, 3) * ShovelerMatrixGet(B, 3, 3));

	return C;
}

static inline ShovelerVector2 shovelerMatrixMultiplyVector2(ShovelerMatrix A, ShovelerVector2 b)
{
	ShovelerVector2 c;
	c.values[0] = ShovelerMatrixGet(A, 0, 0) * b.values[0] + ShovelerMatrixGet(A, 0, 1) * b.values[1];
	c.values[1] = ShovelerMatrixGet(A, 1, 0) * b.values[0] + ShovelerMatrixGet(A, 1, 1) * b.values[1];
	return c;
}

static inline ShovelerVector3 shovelerMatrixMultiplyVector3(ShovelerMatrix A, ShovelerVector3 b)
{
	ShovelerVector3 c;
	c.values[0] = ShovelerMatrixGet(A, 0, 0) * b.values[0] + ShovelerMatrixGet(A, 0, 1) * b.values[1] + ShovelerMatrixGet(A, 0, 2) * b.values[2];
	c.values[1] = ShovelerMatrixGet(A, 1, 0) * b.values[0] + ShovelerMatrixGet(A, 1, 1) * b.values[1] + ShovelerMatrixGet(A, 1, 2) * b.values[2];
	c.values[2] = ShovelerMatrixGet(A, 2, 0) * b.values[0] + ShovelerMatrixGet(A, 2, 1) * b.values[1] + ShovelerMatrixGet(A, 2, 2) * b.values[2];
	return c;
}

static inline ShovelerVector4 shovelerMatrixMultiplyVector4(ShovelerMatrix A, ShovelerVector4 b)
{
	ShovelerVector4 c;
	c.values[0] = ShovelerMatrixGet(A, 0, 0) * b.values[0] + ShovelerMatrixGet(A, 0, 1) * b.values[1] + ShovelerMatrixGet(A, 0, 2) * b.values[2] + ShovelerMatrixGet(A, 0, 3) * b.values[3];
	c.values[1] = ShovelerMatrixGet(A, 1, 0) * b.values[0] + ShovelerMatrixGet(A, 1, 1) * b.values[1] + ShovelerMatrixGet(A, 1, 2) * b.values[2] + ShovelerMatrixGet(A, 1, 3) * b.values[3];
	c.values[2] = ShovelerMatrixGet(A, 2, 0) * b.values[0] + ShovelerMatrixGet(A, 2, 1) * b.values[1] + ShovelerMatrixGet(A, 2, 2) * b.values[2] + ShovelerMatrixGet(A, 2, 3) * b.values[3];
	c.values[2] = ShovelerMatrixGet(A, 3, 0) * b.values[0] + ShovelerMatrixGet(A, 3, 1) * b.values[1] + ShovelerMatrixGet(A, 3, 2) * b.values[2] + ShovelerMatrixGet(A, 3, 3) * b.values[3];
	return c;
}

static inline ShovelerVector3 shovelerVector3Cross(ShovelerVector3 a, ShovelerVector3 b)
{
	ShovelerVector3 c;
	c.values[0] = a.values[1] * b.values[2] - a.values[2] * b.values[1];
	c.values[1] = a.values[2] * b.values[0] - a.values[0] * b.values[2];
	c.values[2] = a.values[0] * b.values[1] - a.values[1] * b.values[0];
	return c;
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

static inline ShovelerVector3 shovelerVector3LinearCombination(float alpha, ShovelerVector3 a, float beta, ShovelerVector3 b)
{
	ShovelerVector3 c;
	c.values[0] = alpha * a.values[0] + beta * b.values[0];
	c.values[1] = alpha * a.values[1] + beta * b.values[1];
	c.values[2] = alpha * a.values[2] + beta * b.values[2];
	return c;
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
	ShovelerMatrixGet(rotation, 0, 1) = xyC - zs;
	ShovelerMatrixGet(rotation, 0, 0) = x * xC + c;
	ShovelerMatrixGet(rotation, 0, 2) = zxC + ys;
	ShovelerMatrixGet(rotation, 1, 0) = xyC + zs;
	ShovelerMatrixGet(rotation, 1, 1) = y * yC + c;
	ShovelerMatrixGet(rotation, 1, 2) = yzC - xs;
	ShovelerMatrixGet(rotation, 2, 0) = zxC - ys;
	ShovelerMatrixGet(rotation, 2, 1) = yzC + xs;
	ShovelerMatrixGet(rotation, 2, 2) = z * zC + c;

	return rotation;
}


#endif
