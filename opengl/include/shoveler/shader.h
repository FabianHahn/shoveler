#ifndef SHOVELER_SHADER_H
#define SHOVELER_SHADER_H

#include <stdbool.h> // bool

#include <glib.h>
#include <glad/glad.h>

#include <shoveler/uniform.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h
struct ShovelerSceneStruct; // forward declaration: scene.h
struct ShovelerShaderStruct; // forward declaration: shader.h

typedef struct {
	struct ShovelerSceneStruct *scene;
	struct ShovelerCameraStruct *camera;
	struct ShovelerLightStruct *light;
	struct ShovelerModelStruct *model;
	struct ShovelerMaterialStruct *material;
	void *userData;
} ShovelerShaderKey;

typedef struct ShovelerShaderStruct {
	ShovelerShaderKey key;
	struct ShovelerMaterialStruct *material;
	/** map from (char *) to (ShovelerUniformAttachment *) */
	GHashTable *attachments;
} ShovelerShader;

/** Computes a hash from a shader key that can be used to add shaders to a hash table. */
guint shovelerShaderKeyHash(gconstpointer shaderKeyPointer);
/** Checks whether two shader keys are equal to be used for a hash table. */
gboolean shovelerShaderKeyEqual(gconstpointer firstShaderKeyPointer, gconstpointer secondShaderKeyPointer);
/** Creates a shader for a material with a previously created shader key. */
ShovelerShader *shovelerShaderCreate(ShovelerShaderKey shaderKey, struct ShovelerMaterialStruct *material);
bool shovelerShaderAttachUniform(ShovelerShader *shader, const char *name, ShovelerUniform *uniform);
bool shovelerShaderUse(ShovelerShader *shader);
void shovelerShaderFree(ShovelerShader *shader);

#endif
