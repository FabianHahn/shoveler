#include <stdlib.h> // malloc, free

#include "log.h"
#include "opengl.h"
#include "uniform.h"

ShovelerUniform *shovelerUniformCreateInt(int value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_INT;
	uniform->value.intValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateIntPointer(int *value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_INT_POINTER;
	uniform->value.intPointerValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateFloat(float value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_FLOAT;
	uniform->value.floatValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateFloatPointer(float *value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_FLOAT_POINTER;
	uniform->value.floatPointerValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateVector2(ShovelerVector2 value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_VECTOR2;
	uniform->value.vector2Value = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateVector2Pointer(ShovelerVector2 *value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_VECTOR2_POINTER;
	uniform->value.vector2PointerValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateVector3(ShovelerVector3 value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_VECTOR3;
	uniform->value.vector3Value = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateVector3Pointer(ShovelerVector3 *value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_VECTOR3_POINTER;
	uniform->value.vector3PointerValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateVector4(ShovelerVector4 value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_VECTOR4;
	uniform->value.vector4Value = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateVector4Pointer(ShovelerVector4 *value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_VECTOR4_POINTER;
	uniform->value.vector4PointerValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateMatrix(ShovelerMatrix value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_MATRIX;
	uniform->value.matrixValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateMatrixPointer(ShovelerMatrix *value)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_MATRIX_POINTER;
	uniform->value.matrixPointerValue = value;
	return uniform;
}

ShovelerUniform *shovelerUniformCopy(const ShovelerUniform *uniform)
{
	ShovelerUniform *newUniform = malloc(sizeof(ShovelerUniform));
	newUniform->type = uniform->type;
	newUniform->value = uniform->value;
	return newUniform;
}

bool shovelerUniformUse(ShovelerUniform *uniform, GLint location)
{
	switch(uniform->type) {
		case SHOVELER_UNIFORM_TYPE_INT:
			glUniform1i(location, uniform->value.intValue);
		break;
		case SHOVELER_UNIFORM_TYPE_INT_POINTER:
			glUniform1i(location, *uniform->value.intPointerValue);
		break;
		case SHOVELER_UNIFORM_TYPE_FLOAT:
			glUniform1f(location, uniform->value.floatValue);
		break;
		case SHOVELER_UNIFORM_TYPE_FLOAT_POINTER:
			glUniform1f(location, *uniform->value.floatPointerValue);
		break;
		case SHOVELER_UNIFORM_TYPE_VECTOR2:
			glUniform2fv(location, 1, uniform->value.vector2Value.values);
		break;
		case SHOVELER_UNIFORM_TYPE_VECTOR2_POINTER:
			glUniform2fv(location, 1, uniform->value.vector2PointerValue->values);
		break;
		case SHOVELER_UNIFORM_TYPE_VECTOR3:
			glUniform3fv(location, 1, uniform->value.vector3Value.values);
		break;
		case SHOVELER_UNIFORM_TYPE_VECTOR3_POINTER:
			glUniform3fv(location, 1, uniform->value.vector3PointerValue->values);
		break;
		case SHOVELER_UNIFORM_TYPE_VECTOR4:
			glUniform4fv(location, 1, uniform->value.vector4Value.values);
		break;
		case SHOVELER_UNIFORM_TYPE_VECTOR4_POINTER:
			glUniform4fv(location, 1, uniform->value.vector4PointerValue->values);
		break;
		case SHOVELER_UNIFORM_TYPE_MATRIX:
			glUniformMatrix4fv(location, 1, GL_TRUE, uniform->value.matrixValue.values);
		break;
		case SHOVELER_UNIFORM_TYPE_MATRIX_POINTER:
			glUniformMatrix4fv(location, 1, GL_TRUE, uniform->value.matrixPointerValue->values);
		break;
	}

	return shovelerOpenGLCheckSuccess();
}

void shovelerUniformFree(ShovelerUniform *uniform)
{
	free(uniform);
}
