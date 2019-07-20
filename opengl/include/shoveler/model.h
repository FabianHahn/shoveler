#ifndef SHOVELER_MODEL_H
#define SHOVELER_MODEL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include <shoveler/drawable.h>
#include <shoveler/types.h>
#include <shoveler/uniform_map.h>

struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

typedef struct ShovelerModelStruct {
	struct ShovelerShaderCacheStruct *shaderCache;
	ShovelerDrawable *drawable;
	struct ShovelerMaterialStruct *material;
	ShovelerVector3 translation;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	ShovelerMatrix transformation;
	ShovelerMatrix normalTransformation;
	bool visible;
	bool emitter;
	bool castsShadow;
	GLuint polygonMode;
	ShovelerUniformMap *uniforms;
} ShovelerModel;

ShovelerModel *shovelerModelCreate(ShovelerDrawable *drawable, struct ShovelerMaterialStruct *material);
void shovelerModelUpdateTransformation(ShovelerModel *model);
bool shovelerModelRender(ShovelerModel *model);
void shovelerModelFree(ShovelerModel *model);

#endif
