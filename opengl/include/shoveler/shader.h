#ifndef SHOVELER_SHADER_H
#define SHOVELER_SHADER_H

#include <stdbool.h> // bool

#include <glib.h>
#include <glad/glad.h>

#include <shoveler/material.h>
#include <shoveler/uniform.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerModelStruct; // forward declaration: model.h
struct ShovelerSceneStruct; // forward declaration: scene.h
struct ShovelerShaderStruct; // forward declaration: shader.h

typedef struct ShovelerShaderStruct {
	ShovelerMaterial *material;
	guint hash;
	/** map from (char *) to (ShovelerUniformAttachment *) */
	GHashTable *attachments;
} ShovelerShader;

/** Computes a hash that can be used to create a shader for a (scene, camera, light, model, material, userData) tuple. */
guint shovelerShaderComputeHash(struct ShovelerSceneStruct *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, ShovelerMaterial *material, void *userData);
/** Creates a shader for a material with a hash previously returned by shovelerShaderComputeHash. */
ShovelerShader *shovelerShaderCreate(ShovelerMaterial *material, guint hash);
bool shovelerShaderAttachUniform(ShovelerShader *shader, const char *name, ShovelerUniform *uniform);
bool shovelerShaderUse(ShovelerShader *shader);
void shovelerShaderFree(ShovelerShader *shader);

#endif
