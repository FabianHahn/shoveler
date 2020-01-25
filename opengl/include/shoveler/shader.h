#ifndef SHOVELER_SHADER_H
#define SHOVELER_SHADER_H

#include <stdbool.h> // bool

#include <glib.h>
#include <glad/glad.h>

#include <shoveler/uniform.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerShaderStruct ShovelerShader; // forward declaration: shader.h

typedef struct ShovelerShaderKeyStruct {
	ShovelerScene *scene;
	ShovelerCamera *camera;
	ShovelerLight *light;
	ShovelerModel *model;
	ShovelerMaterial *material;
	void *userData;
} ShovelerShaderKey;

typedef struct ShovelerShaderStruct {
	ShovelerShaderKey key;
	ShovelerMaterial *material;
	/** map from (char *) to (ShovelerUniformAttachment *) */
	GHashTable *attachments;
} ShovelerShader;

/** Computes a hash from a shader key that can be used to add shaders to a hash table. */
guint shovelerShaderKeyHash(gconstpointer shaderKeyPointer);
/** Checks whether two shader keys are equal to be used for a hash table. */
gboolean shovelerShaderKeyEqual(gconstpointer firstShaderKeyPointer, gconstpointer secondShaderKeyPointer);
/** Creates a shader for a material with a previously created shader key. */
ShovelerShader *shovelerShaderCreate(ShovelerShaderKey shaderKey, ShovelerMaterial *material);
bool shovelerShaderAttachUniform(ShovelerShader *shader, const char *name, ShovelerUniform *uniform);
bool shovelerShaderUse(ShovelerShader *shader);
void shovelerShaderFree(ShovelerShader *shader);

#endif
