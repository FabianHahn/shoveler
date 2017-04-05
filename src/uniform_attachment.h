#ifndef SHOVELER_UNIFORM_ATTACHMENT_H
#define SHOVELER_UNIFORM_ATTACHMENT_H

#include "uniform.h"

typedef struct {
	ShovelerUniform *uniform;
	GLint location;
} ShovelerUniformAttachment;

ShovelerUniformAttachment *shovelerUniformAttachmentCreate(ShovelerUniform *uniform, GLint location);
bool shovelerUniformAttachmentUse(ShovelerUniformAttachment *uniformAttachment, GLint *textureUnitIndexCounter);
void shovelerUniformAttachmentFree(ShovelerUniformAttachment *uniformAttachment);

#endif
