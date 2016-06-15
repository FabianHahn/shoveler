#ifndef SHOVELER_SHADER_H
#define SHOVELER_SHADER_H

#include <stdbool.h> // bool

#include <glib.h>
#include <glad/glad.h>

#include "uniform.h"
#include "uniform_map.h"

typedef struct {
	GLuint program;
	GHashTable *attachments;
} ShovelerShader;

ShovelerShader *shovelerShaderCreate(GLuint program);
int shovelerShaderAttachUniforms(ShovelerShader *shader, ShovelerUniformMap *uniformMap);
bool shovelerShaderUse(ShovelerShader *shader);
void shovelerShaderFree(ShovelerShader *shader);

#endif
