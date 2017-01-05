#include <assert.h> // assert
#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free
#include <string.h> // strdup

#include "log.h"
#include "opengl.h"
#include "shader.h"
#include "uniform_attachment.h"

void freeAttachment(void *attachmentPointer);

ShovelerShader *shovelerShaderCreate(ShovelerMaterial *material)
{
	ShovelerShader *shader = malloc(sizeof(ShovelerShader));
	shader->material = material;
	shader->attachments = g_hash_table_new_full(g_str_hash, g_str_equal, free, freeAttachment);
	return shader;
}

int shovelerShaderAttachUniforms(ShovelerShader *shader, ShovelerUniformMap *uniformMap)
{
	int attached = 0;

	GHashTableIter iter;
	char *name;
	ShovelerUniform *uniform;
	g_hash_table_iter_init(&iter, uniformMap->uniforms);
	while(g_hash_table_iter_next(&iter, (gpointer *) &name, (gpointer *) &uniform)) {
		GLint location = glGetUniformLocation(shader->material->program, name);
		if(location < 0 || !shovelerOpenGLCheckSuccess()) {
			shovelerLogWarning("Failed to get uniform location for '%s' when trying to attach uniforms to shader program %d, skipping.", name, shader->material->program);
			continue;
		}

		if(g_hash_table_contains(shader->attachments, name)) {
			shovelerLogWarning("Shader already contains an attachment for '%s', skipping.", name);
			continue;
		}

		ShovelerUniformAttachment *uniformAttachment = shovelerUniformAttachmentCreate(uniform, location);
		g_hash_table_insert(shader->attachments, strdup(name), uniformAttachment);

		shovelerLogTrace("Attached uniform '%s' to shader program %d.", name, shader->material->program);

		attached++;
	}

	return attached;
}

bool shovelerShaderUse(ShovelerShader *shader)
{
	glUseProgram(shader->material->program);

	GLint unitIndex = 0;
	GList *samplerIter = shader->material->samplers->head;
	for(GList *iter = shader->material->textures->head; iter != NULL; iter = iter->next, samplerIter = samplerIter->next, unitIndex++) {
		assert(samplerIter != NULL);

		ShovelerTexture *texture = iter->data;
		if(!shovelerTextureUse(texture, unitIndex)) {
			shovelerLogError("Failed to bind texture %p when trying to use shader", texture);
			return false;
		}

		ShovelerSampler *sampler = samplerIter->data;
		if(!shovelerSamplerUse(sampler, unitIndex)) {
			shovelerLogError("Failed to bind sampler %p when trying to use shader", sampler);
			return false;
		}
	}

	GHashTableIter iter;
	char *name;
	ShovelerUniformAttachment *uniformAttachment;
	g_hash_table_iter_init(&iter, shader->attachments);
	while(g_hash_table_iter_next(&iter, (gpointer *) &name, (gpointer *) &uniformAttachment)) {
		if(!shovelerUniformAttachmentUse(uniformAttachment)) {
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

void freeAttachment(void *attachmentPointer)
{
	shovelerUniformAttachmentFree(attachmentPointer);
}
