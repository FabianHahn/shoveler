#ifndef SHOVELER_SHADER_H
#define SHOVELER_SHADER_H

#include <stdbool.h> // bool

#include <glib.h>
#include <glad/glad.h>

#include <shoveler/material.h>
#include <shoveler/uniform.h>

typedef struct ShovelerShaderStruct {
	ShovelerMaterial *material;
	/** map from (char *) to (ShovelerUniformAttachment *) */
	GHashTable *attachments;
} ShovelerShader;

ShovelerShader *shovelerShaderCreate(ShovelerMaterial *material);
bool shovelerShaderAttachUniform(ShovelerShader *shader, const char *name, ShovelerUniform *uniform);
bool shovelerShaderUse(ShovelerShader *shader);
void shovelerShaderFree(ShovelerShader *shader);

#endif
