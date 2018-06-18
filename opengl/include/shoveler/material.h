#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include <shoveler/sampler.h>
#include <shoveler/texture.h>
#include <shoveler/uniform_map.h>

struct ShovelerMaterialStruct;
struct ShovelerShaderStruct; // forward declaration: shader.h

typedef int (ShovelerMaterialAttachUniformsFunction)(struct ShovelerMaterialStruct *material, struct ShovelerShaderStruct *shader);
typedef void (ShovelerMaterialFreeDataFunction)(struct ShovelerMaterialStruct *material);

typedef struct ShovelerMaterialStruct {
	bool manageProgram;
	GLuint program;
	/** do not access directly - use shovelerMaterialAttachUniforms instead */
	ShovelerUniformMap *uniforms;
	ShovelerMaterialAttachUniformsFunction *attachUniforms;
	ShovelerMaterialFreeDataFunction *freeData;
	void *data;
} ShovelerMaterial;

/** Creates a material from a program that is then owned by the object. */
ShovelerMaterial *shovelerMaterialCreate(GLuint program);
/** Creates a material from a program without owning it. */
ShovelerMaterial *shovelerMaterialCreateUnmanaged(GLuint program);
void shovelerMaterialFree(ShovelerMaterial *material);

static inline int shovelerMaterialAttachUniforms(ShovelerMaterial *material, struct ShovelerShaderStruct *shader)
{
	return material->attachUniforms(material, shader);
}

#endif
