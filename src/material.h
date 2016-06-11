#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <GL/glew.h>

#include "uniform_map.h"

typedef struct {
	GLuint program;
	ShovelerUniformMap *uniforms;
} ShovelerMaterial;

ShovelerMaterial *shovelerMaterialCreate(GLuint program);
void shovelerMaterialFree(ShovelerMaterial *material);

#endif
