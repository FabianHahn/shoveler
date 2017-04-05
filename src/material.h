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
	ShovelerMaterialFreeDataFunction *freeData;
	void *data;
} ShovelerMaterial;

ShovelerMaterial *shovelerMaterialCreate(GLuint program);
void shovelerMaterialFree(ShovelerMaterial *material);

#endif
