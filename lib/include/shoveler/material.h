#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <glad/glad.h>

#include <shoveler/sampler.h>
#include <shoveler/texture.h>
#include <shoveler/uniform_map.h>

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
