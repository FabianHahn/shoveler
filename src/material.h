#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <glad/glad.h>

#include "sampler.h"
#include "texture.h"
#include "uniform_map.h"

typedef struct {
	GLuint program;
	ShovelerUniformMap *uniforms;
	GQueue *textures;
	GQueue *samplers;
} ShovelerMaterial;

ShovelerMaterial *shovelerMaterialCreate(GLuint program);
bool shovelerMaterialAttachTexture(ShovelerMaterial *material, const char *name, ShovelerTexture *texture, ShovelerSampler *sampler);
void shovelerMaterialFree(ShovelerMaterial *material);

#endif
