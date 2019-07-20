#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: below
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerRenderStateStruct ShovelerRenderState; // forward declaration: render_state.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: shader.h
typedef struct ShovelerShaderStruct ShovelerShader; // forward declaration: shader.h
typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerUniformMapStruct ShovelerUniformMap; // forward declaration: uniform_map.h

typedef bool (ShovelerMaterialRenderFunction)(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
typedef int (ShovelerMaterialAttachUniformsFunction)(ShovelerMaterial *material, ShovelerShader *shader, void *userData);
typedef void (ShovelerMaterialFreeDataFunction)(ShovelerMaterial *material);

typedef struct ShovelerMaterialStruct {
	ShovelerShaderCache *shaderCache;
	bool screenspace;
	bool manageProgram;
	GLuint program;
	/** do not access directly - use shovelerMaterialAttachUniforms instead */
	ShovelerUniformMap *uniforms;
	/** callback executed whenever a model is rendered with this material */
	ShovelerMaterialRenderFunction *render;
	/** callback executed whenever a render call causes a shader generation */
	ShovelerMaterialAttachUniformsFunction *attachUniforms;
	ShovelerMaterialFreeDataFunction *freeData;
	void *data;
} ShovelerMaterial;

/** Creates a material from a program that is then owned by the object. */
ShovelerMaterial *shovelerMaterialCreate(ShovelerShaderCache *shaderCache, bool screenspace, GLuint program);
/** Creates a material from a program without owning it. */
ShovelerMaterial *shovelerMaterialCreateUnmanaged(ShovelerShaderCache *shaderCache, bool screenspace, GLuint program);
void shovelerMaterialFree(ShovelerMaterial *material);

static inline bool shovelerMaterialRender(ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	return material->render(material, scene, camera, light, model, renderState);
}

static inline int shovelerMaterialAttachUniforms(ShovelerMaterial *material, ShovelerShader *shader, void *userData)
{
	return material->attachUniforms(material, shader, userData);
}

#endif
