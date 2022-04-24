#include "shoveler/uniform_attachment.h"

#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

ShovelerUniformAttachment* shovelerUniformAttachmentCreate(
    ShovelerUniform* uniform, GLint location) {
  ShovelerUniformAttachment* uniformAttachment = malloc(sizeof(ShovelerUniformAttachment));
  uniformAttachment->uniform = uniform;
  uniformAttachment->location = location;
  return uniformAttachment;
}

bool shovelerUniformAttachmentUse(
    ShovelerUniformAttachment* uniformAttachment, GLuint* textureUnitIndexCounter) {
  return shovelerUniformUse(
      uniformAttachment->uniform, uniformAttachment->location, textureUnitIndexCounter);
}

void shovelerUniformAttachmentFree(ShovelerUniformAttachment* uniformAttachment) {
  free(uniformAttachment);
}
