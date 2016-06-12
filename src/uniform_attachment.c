#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include "uniform_attachment.h"

ShovelerUniformAttachment *shovelerUniformAttachmentCreate(ShovelerUniform *uniform, GLint location)
{
	ShovelerUniformAttachment *uniformAttachment = malloc(sizeof(ShovelerUniform));
	uniformAttachment->uniform = uniform;
	uniformAttachment->location = location;
}

bool shovelerUniformAttachmentUse(ShovelerUniformAttachment *uniformAttachment)
{
	return shovelerUniformUse(uniformAttachment->uniform, uniformAttachment->location);
}

void shovelerUniformAttachmentFree(ShovelerUniformAttachment *uniformAttachment)
{
	free(uniformAttachment);
}
