#include <stdlib.h> // malloc, free

#include <glib.h>

#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/shader.h"
#include "shoveler/uniform.h"

static int attachUniforms(ShovelerMaterial *material, ShovelerShader *shader);
static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialCreate(GLuint program)
{
	ShovelerMaterial *material = malloc(sizeof(ShovelerMaterial));
	material->manageProgram = true;
	material->program = program;
	material->uniforms = shovelerUniformMapCreate();
	material->attachUniforms = attachUniforms;
	material->freeData = freeMaterialData;
	material->data = NULL;
	return material;
}

ShovelerMaterial *shovelerMaterialCreateUnmanaged(GLuint program)
{
	ShovelerMaterial *material = malloc(sizeof(ShovelerMaterial));
	material->manageProgram = false;
	material->program = program;
	material->uniforms = shovelerUniformMapCreate();
	material->freeData = freeMaterialData;
	material->data = NULL;
	return material;
}

void shovelerMaterialFree(ShovelerMaterial *material)
{
	if(material == NULL) {
		return;
	}

	material->freeData(material);
	shovelerUniformMapFree(material->uniforms);

	if(material->manageProgram) {
		glDeleteProgram(material->program);
	}

	free(material);
}

static int attachUniforms(ShovelerMaterial *material, ShovelerShader *shader)
{
	// simply attach uniform map by default
	return shovelerUniformMapAttach(material->uniforms, shader);
}

static void freeMaterialData(ShovelerMaterial *material)
{
	// do nothing by default
}
