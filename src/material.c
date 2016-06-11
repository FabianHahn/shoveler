#include <stdlib.h> // malloc, free

#include "material.h"

ShovelerMaterial *shovelerMaterialCreate(GLuint program)
{
	ShovelerMaterial *material = malloc(sizeof(ShovelerMaterial));
	material->program = program;
	material->uniforms = shovelerUniformMapCreate();
	return material;
}


void shovelerMaterialFree(ShovelerMaterial *material)
{
	shovelerUniformMapFree(material->uniforms);
	glDeleteProgram(material->program);
	free(material);
}
