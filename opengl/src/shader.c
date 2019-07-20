#include <assert.h> // assert
#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free
#include <string.h> // strdup, memcmp

#include "shoveler/camera.h"
#include "shoveler/hash.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/opengl.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"
#include "shoveler/uniform_attachment.h"

static void freeAttachment(void *attachmentPointer);

guint shovelerShaderKeyHash(gconstpointer shaderKeyPointer)
{
	ShovelerShaderKey *shaderKey = (ShovelerShaderKey *) shaderKeyPointer;

	guint sceneHash = g_direct_hash(shaderKey->scene);
	guint cameraHash = g_direct_hash(shaderKey->camera);
	guint lightHash = g_direct_hash(shaderKey->light);
	guint modelHash = g_direct_hash(shaderKey->model);
	guint materialHash = g_direct_hash(shaderKey->material);
	guint userDataHash = g_direct_hash(shaderKey->userData);

	return shovelerHashCombine(sceneHash, shovelerHashCombine(cameraHash, shovelerHashCombine(lightHash, shovelerHashCombine(modelHash, shovelerHashCombine(materialHash, userDataHash)))));
}

gboolean shovelerShaderKeyEqual(gconstpointer firstShaderKeyPointer, gconstpointer secondShaderKeyPointer)
{
	return memcmp(firstShaderKeyPointer, secondShaderKeyPointer, sizeof(ShovelerShaderKey)) == 0;
}

ShovelerShader *shovelerShaderCreate(ShovelerShaderKey shaderKey, ShovelerMaterial *material)
{
	ShovelerShader *shader = malloc(sizeof(ShovelerShader));
	shader->key = shaderKey;
	shader->material = material;
	shader->attachments = g_hash_table_new_full(g_str_hash, g_str_equal, free, freeAttachment);
	return shader;
}

bool shovelerShaderAttachUniform(ShovelerShader *shader, const char *name, ShovelerUniform *uniform)
{
	GLint location = glGetUniformLocation(shader->material->program, name);
	if(!shovelerOpenGLCheckSuccess()) {
		return false;
	} else if(location < 0) {
		shovelerLogTrace("Material %p with shader program %d does not have a uniform attachment point for '%s', skipping.", shader->material, shader->material->program, name);
		return false;
	} else if(g_hash_table_contains(shader->attachments, name)) {
		shovelerLogTrace("Material %p with shader program %d already contains an attachment for '%s', skipping.", shader->material, shader->material->program, name);
		return false;
	}

	ShovelerUniformAttachment *uniformAttachment = shovelerUniformAttachmentCreate(uniform, location);
	g_hash_table_insert(shader->attachments, strdup(name), uniformAttachment);

	shovelerLogTrace("Attached uniform '%s' to material %p with shader program %d.", name, shader->material, shader->material->program);
	return true;
}

bool shovelerShaderUse(ShovelerShader *shader)
{
	glUseProgram(shader->material->program);

	GLuint textureUnitIndexCounter = 0;
	GHashTableIter iter;
	char *name;
	ShovelerUniformAttachment *uniformAttachment;
	g_hash_table_iter_init(&iter, shader->attachments);
	while(g_hash_table_iter_next(&iter, (gpointer *) &name, (gpointer *) &uniformAttachment)) {
		if(!shovelerUniformAttachmentUse(uniformAttachment, &textureUnitIndexCounter)) {
			shovelerLogError("Failed to use uniform attachment '%s' when trying to use shader", name);
			return false;
		}
	}
	return shovelerOpenGLCheckSuccess();
}

void shovelerShaderFree(ShovelerShader *shader)
{
	g_hash_table_destroy(shader->attachments);
	free(shader);
}

static void freeAttachment(void *attachmentPointer)
{
	shovelerUniformAttachmentFree(attachmentPointer);
}
