#ifndef SHOVELER_FRAMEBUFFER_H
#define SHOVELER_FRAMEBUFFER_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include "texture.h"

typedef struct {
	GLuint framebuffer;
	GLsizei width;
	GLsizei height;
	ShovelerTexture *renderTarget;
	ShovelerTexture *depthTarget;
} ShovelerFramebuffer;

ShovelerFramebuffer *shovelerFramebufferCreate(GLsizei width, GLsizei height);
ShovelerFramebuffer *shovelerFramebufferCreateMultisample(GLsizei width, GLsizei height, GLsizei samples);
ShovelerFramebuffer *shovelerFramebufferCreateDepthOnly(GLsizei width, GLsizei height);
bool shovelerFramebufferUse(ShovelerFramebuffer *framebuffer);
bool shovelerFramebufferBlitToDefault(ShovelerFramebuffer *framebuffer);
void shovelerFramebufferFree(ShovelerFramebuffer *framebuffer);

#endif