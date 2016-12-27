#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <glad/glad.h>

#include "sampler.h"
#include "texture.h"
#include "uniform_map.h"

struct ShovelerMaterialStruct;

typedef void (ShovelerMaterialFreeDataFunction)(struct ShovelerMaterialStruct *material);

typedef struct ShovelerMaterialStruct {
	GLuint program;
	ShovelerUniformMap *uniforms;
	GQueue *textures;
	GQueue *samplers;
	ShovelerMaterialFreeDataFunction *freeData;
	void *data;
} ShovelerMaterial;

ShovelerMaterial *shovelerMaterialCreate(GLuint program);
bool shovelerMaterialAttachTexture(ShovelerMaterial *material, const char *name, ShovelerTexture *texture, ShovelerSampler *sampler);
void shovelerMaterialFree(ShovelerMaterial *material);

#endif
