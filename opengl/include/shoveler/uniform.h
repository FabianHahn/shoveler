#ifndef SHOVELER_UNIFORM_H
#define SHOVELER_UNIFORM_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include <shoveler/sampler.h>
#include <shoveler/texture.h>
#include <shoveler/types.h>

typedef enum {
	SHOVELER_UNIFORM_TYPE_BOOL,
	SHOVELER_UNIFORM_TYPE_BOOL_POINTER,
	SHOVELER_UNIFORM_TYPE_INT,
	SHOVELER_UNIFORM_TYPE_INT_POINTER,
	SHOVELER_UNIFORM_TYPE_UNSIGNED_INT,
	SHOVELER_UNIFORM_TYPE_UNSIGNED_INT_POINTER,
	SHOVELER_UNIFORM_TYPE_FLOAT,
	SHOVELER_UNIFORM_TYPE_FLOAT_POINTER,
	SHOVELER_UNIFORM_TYPE_VECTOR2,
	SHOVELER_UNIFORM_TYPE_VECTOR2_POINTER,
	SHOVELER_UNIFORM_TYPE_VECTOR3,
	SHOVELER_UNIFORM_TYPE_VECTOR3_POINTER,
	SHOVELER_UNIFORM_TYPE_VECTOR4,
	SHOVELER_UNIFORM_TYPE_VECTOR4_POINTER,
	SHOVELER_UNIFORM_TYPE_MATRIX,
	SHOVELER_UNIFORM_TYPE_MATRIX_POINTER,
	SHOVELER_UNIFORM_TYPE_TEXTURE,
	SHOVELER_UNIFORM_TYPE_TEXTURE_POINTER
} ShovelerUniformType;

typedef struct {
	ShovelerTexture *texture;
	ShovelerSampler *sampler;
} ShovelerUniformTexture;

typedef struct {
	ShovelerTexture **texturePointer;
	ShovelerSampler **samplerPointer;
} ShovelerUniformTexturePointer;

typedef union {
	bool boolValue;
	bool *boolPointerValue;
	int intValue;
	int *intPointerValue;
	unsigned int unsignedIntValue;
	unsigned int *unsignedIntPointerValue;
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
	ShovelerUniformTexture textureValue;
	ShovelerUniformTexturePointer texturePointerValue;
} ShovelerUniformValue;

typedef struct {
	ShovelerUniformType type;
	ShovelerUniformValue value;
} ShovelerUniform;

ShovelerUniform *shovelerUniformCreateBool(bool value);
ShovelerUniform *shovelerUniformCreateBoolPointer(bool *value);
ShovelerUniform *shovelerUniformCreateInt(int value);
ShovelerUniform *shovelerUniformCreateIntPointer(int *value);
ShovelerUniform *shovelerUniformCreateUnsignedInt(unsigned int value);
ShovelerUniform *shovelerUniformCreateUnsignedIntPointer(unsigned int *value);
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
ShovelerUniform *shovelerUniformCreateTexture(ShovelerTexture *texture, ShovelerSampler *sampler);
ShovelerUniform *shovelerUniformCreateTexturePointer(ShovelerTexture **texturePointer, ShovelerSampler **samplerPointer);
ShovelerUniform *shovelerUniformCopy(const ShovelerUniform *uniform);
bool shovelerUniformUse(ShovelerUniform *uniform, GLint location, GLuint *textureUnitIndexCounter);
void shovelerUniformFree(ShovelerUniform *uniform);

#endif
