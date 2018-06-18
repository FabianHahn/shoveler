#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include <shoveler/sampler.h>
#include <shoveler/texture.h>
#include <shoveler/uniform_map.h>

struct ShovelerMaterialStruct;

typedef void (ShovelerMaterialFreeDataFunction)(struct ShovelerMaterialStruct *material);

typedef struct ShovelerMaterialStruct {
	bool manageProgram;
	GLuint program;
	ShovelerUniformMap *uniforms;
	ShovelerMaterialFreeDataFunction *freeData;
	void *data;
} ShovelerMaterial;

/** Creates a material from a program that is then owned by the object. */
ShovelerMaterial *shovelerMaterialCreate(GLuint program);
/** Creates a material from a program without owning it. */
ShovelerMaterial *shovelerMaterialCreateUnmanaged(GLuint program);
void shovelerMaterialFree(ShovelerMaterial *material);

#endif
