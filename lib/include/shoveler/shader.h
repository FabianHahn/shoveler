#ifndef SHOVELER_SHADER_H
#define SHOVELER_SHADER_H

#include <stdbool.h> // bool

#include <glib.h>
#include <glad/glad.h>

#include <shoveler/material.h>
#include <shoveler/uniform.h>
#include <shoveler/uniform_map.h>

typedef struct {
	ShovelerMaterial *material;
	GHashTable *attachments;
} ShovelerShader;

ShovelerShader *shovelerShaderCreate(ShovelerMaterial *material);
int shovelerShaderAttachUniforms(ShovelerShader *shader, ShovelerUniformMap *uniformMap);
bool shovelerShaderUse(ShovelerShader *shader);
void shovelerShaderFree(ShovelerShader *shader);

#endif
