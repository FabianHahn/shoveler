#ifndef SHOVELER_UNIFORM_H
#define SHOVELER_UNIFORM_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include "types.h"

typedef enum {
	SHOVELER_UNIFORM_TYPE_INT,
	SHOVELER_UNIFORM_TYPE_INT_POINTER,
	SHOVELER_UNIFORM_TYPE_FLOAT,
	SHOVELER_UNIFORM_TYPE_FLOAT_POINTER,
	SHOVELER_UNIFORM_TYPE_VECTOR2,
	SHOVELER_UNIFORM_TYPE_VECTOR2_POINTER,
	SHOVELER_UNIFORM_TYPE_VECTOR3,
	SHOVELER_UNIFORM_TYPE_VECTOR3_POINTER,
	SHOVELER_UNIFORM_TYPE_VECTOR4,
	SHOVELER_UNIFORM_TYPE_VECTOR4_POINTER,
	SHOVELER_UNIFORM_TYPE_MATRIX,
	SHOVELER_UNIFORM_TYPE_MATRIX_POINTER
} ShovelerUniformType;

typedef union {
	int intValue;
	int *intPointerValue;
	float floatValue;
	float *floatPointerValue;
	ShovelerVector2 vector2Value;
	ShovelerVector2 *vector2PointerValue;
	ShovelerVector3 vector3Value;
	ShovelerVector3 *vector3PointerValue;
	ShovelerVector4 vector4Value;
	ShovelerVector4 *vector4PointerValue;
	ShovelerMatrix matrixValue;
	ShovelerMatrix *matrixPointerValue;
} ShovelerUniformValue;

typedef struct {
	ShovelerUniformType type;
	ShovelerUniformValue value;
} ShovelerUniform;

ShovelerUniform *shovelerUniformCreateInt(int value);
ShovelerUniform *shovelerUniformCreateIntPointer(int *value);
ShovelerUniform *shovelerUniformCreateFloat(float value);
ShovelerUniform *shovelerUniformCreateFloatPointer(float *value);
ShovelerUniform *shovelerUniformCreateVector2(ShovelerVector2 value);
ShovelerUniform *shovelerUniformCreateVector2Pointer(ShovelerVector2 *value);
ShovelerUniform *shovelerUniformCreateVector3(ShovelerVector3 value);
ShovelerUniform *shovelerUniformCreateVector3Pointer(ShovelerVector3 *value);
ShovelerUniform *shovelerUniformCreateVector4(ShovelerVector4 value);
ShovelerUniform *shovelerUniformCreateVector4Pointer(ShovelerVector4 *value);
ShovelerUniform *shovelerUniformCreateMatrix(ShovelerMatrix value);
ShovelerUniform *shovelerUniformCreateMatrixPointer(ShovelerMatrix *value);
ShovelerUniform *shovelerUniformCopy(const ShovelerUniform *uniform);
bool shovelerUniformUse(ShovelerUniform *uniform, GLint location);
void shovelerUniformFree(ShovelerUniform *uniform);

#endif
