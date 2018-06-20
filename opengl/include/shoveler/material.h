#ifndef SHOVELER_MATERIAL_H
#define SHOVELER_MATERIAL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include <shoveler/sampler.h>
#include <shoveler/texture.h>
#include <shoveler/uniform_map.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct;
struct ShovelerModelStruct; // forward declaration: model.h
struct ShovelerSceneStruct; // forward declaration: scene.h
struct ShovelerShaderStruct; // forward declaration: shader.h

typedef bool (ShovelerMaterialRenderFunction)(struct ShovelerMaterialStruct *material, struct ShovelerSceneStruct *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model);
typedef int (ShovelerMaterialAttachUniformsFunction)(struct ShovelerMaterialStruct *material, struct ShovelerShaderStruct *shader, void *userData);
typedef void (ShovelerMaterialFreeDataFunction)(struct ShovelerMaterialStruct *material);

typedef struct ShovelerMaterialStruct {
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
ShovelerMaterial *shovelerMaterialCreate(GLuint program);
/** Creates a material from a program without owning it. */
ShovelerMaterial *shovelerMaterialCreateUnmanaged(GLuint program);
void shovelerMaterialFree(ShovelerMaterial *material);

static inline bool shovelerMaterialRender(ShovelerMaterial *material, struct ShovelerSceneStruct *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model)
{
	return material->render(material, scene, camera, light, model);
}

static inline int shovelerMaterialAttachUniforms(ShovelerMaterial *material, struct ShovelerShaderStruct *shader, void *userData)
{
	return material->attachUniforms(material, shader, userData);
}

#endif
