#include <stdlib.h> // malloc, free

#include "shoveler/log.h"
#include "shoveler/opengl.h"
#include "shoveler/uniform.h"

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

ShovelerUniform *shovelerUniformCreateTexture(ShovelerTexture *texture, ShovelerSampler *sampler)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_TEXTURE;
	uniform->value.textureValue.texture = texture;
	uniform->value.textureValue.sampler = sampler;
	return uniform;
}

ShovelerUniform *shovelerUniformCreateTexturePointer(ShovelerTexture **texturePointer, ShovelerSampler **samplerPointer)
{
	ShovelerUniform *uniform = malloc(sizeof(ShovelerUniform));
	uniform->type = SHOVELER_UNIFORM_TYPE_TEXTURE_POINTER;
	uniform->value.texturePointerValue.texturePointer = texturePointer;
	uniform->value.texturePointerValue.samplerPointer = samplerPointer;
	return uniform;
}

ShovelerUniform *shovelerUniformCopy(const ShovelerUniform *uniform)
{
	ShovelerUniform *newUniform = malloc(sizeof(ShovelerUniform));
	newUniform->type = uniform->type;
	newUniform->value = uniform->value;
	return newUniform;
}

bool shovelerUniformUse(ShovelerUniform *uniform, GLint location, GLuint *textureUnitIndexCounter)
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
		case SHOVELER_UNIFORM_TYPE_TEXTURE: {
			GLuint textureUnitIndex = (*textureUnitIndexCounter)++;

			if(!shovelerTextureUse(uniform->value.textureValue.texture, textureUnitIndex)) {
				shovelerLogError("Failed to use texture %p at unit index %d when trying to use texture uniform %d at location %d.", uniform->value.textureValue.texture, textureUnitIndex, uniform, location);
				return false;
			}

			if(!shovelerSamplerUse(uniform->value.textureValue.sampler, textureUnitIndex)) {
				shovelerLogError("Failed to use sampler %p at unit index %d when trying to use texture uniform %d at location %d.", uniform->value.textureValue.sampler, textureUnitIndex, uniform, location);
				return false;
			}

			glUniform1i(location, textureUnitIndex);
		} break;
		case SHOVELER_UNIFORM_TYPE_TEXTURE_POINTER: {
			GLuint textureUnitIndex = (*textureUnitIndexCounter)++;

			if(!shovelerTextureUse(*uniform->value.texturePointerValue.texturePointer, textureUnitIndex)) {
				shovelerLogError("Failed to use texture pointer %p at unit index %d when trying to use texture pointer uniform %d at location %d.", uniform->value.texturePointerValue.texturePointer, textureUnitIndex, uniform, location);
				return false;
			}

			if(!shovelerSamplerUse(*uniform->value.texturePointerValue.samplerPointer, textureUnitIndex)) {
				shovelerLogError("Failed to use sampler pointer %p at unit index %d when trying to use texture pointer uniform %d at location %d.", uniform->value.texturePointerValue.samplerPointer, textureUnitIndex, uniform, location);
				return false;
			}

			glUniform1i(location, textureUnitIndex);
		} break;
	}

	return shovelerOpenGLCheckSuccess();
}

void shovelerUniformFree(ShovelerUniform *uniform)
{
	free(uniform);
}
