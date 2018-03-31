#include <assert.h> // assert
#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free
#include <string.h> // strdup

#include "shoveler/log.h"
#include "shoveler/opengl.h"
#include "shoveler/shader.h"
#include "shoveler/uniform_attachment.h"

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
		if(!shovelerOpenGLCheckSuccess()) {
			continue;
		} else if(location < 0) {
			shovelerLogTrace("Material %p with shader program %d does not have a uniform attachment point for '%s', skipping.", shader->material, shader->material->program, name);
			continue;
		} else if(g_hash_table_contains(shader->attachments, name)) {
			shovelerLogWarning("Material %p with shader program %d already contains an attachment for '%s', skipping.", shader->material, shader->material->program, name);
			continue;
		}

		ShovelerUniformAttachment *uniformAttachment = shovelerUniformAttachmentCreate(uniform, location);
		g_hash_table_insert(shader->attachments, strdup(name), uniformAttachment);

		shovelerLogTrace("Attached uniform '%s' to material %p with shader program %d.", name, shader->material, shader->material->program);

		attached++;
	}

	return attached;
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

void freeAttachment(void *attachmentPointer)
{
	shovelerUniformAttachmentFree(attachmentPointer);
}
