#include <stdlib.h> // malloc, free

#include <glib.h>

#include "log.h"
#include "material.h"
#include "uniform.h"

static void freeMaterialData(ShovelerMaterial *material);

ShovelerMaterial *shovelerMaterialCreate(GLuint program)
{
	ShovelerMaterial *material = malloc(sizeof(ShovelerMaterial));
	material->program = program;
	material->uniforms = shovelerUniformMapCreate();
	material->textures = g_queue_new();
	material->samplers = g_queue_new();
	material->freeData = freeMaterialData;
	material->data = NULL;
	return material;
}

bool shovelerMaterialAttachTexture(ShovelerMaterial *material, const char *name, ShovelerTexture *texture, ShovelerSampler *sampler)
{
	ShovelerUniform *uniform = shovelerUniformCreateTextureUnitIndex(material->textures->length);
	if(!shovelerUniformMapInsert(material->uniforms, name, uniform)) {
		shovelerLogError("Failed to attach texture %p to material %p with name '%s'", texture, material, name);
		shovelerUniformFree(uniform);
		return false;
	}

	g_queue_push_tail(material->textures, texture);
	g_queue_push_tail(material->samplers, sampler);
	return true;
}

void shovelerMaterialFree(ShovelerMaterial *material)
{
	material->freeData(material);
	shovelerUniformMapFree(material->uniforms);
	g_queue_free(material->samplers);
	g_queue_free(material->textures);
	glDeleteProgram(material->program);
	free(material);
}

static void freeMaterialData(ShovelerMaterial *material)
{
	// do nothing by default
}
