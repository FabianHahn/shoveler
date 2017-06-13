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

ShovelerFramebuffer *shovelerFramebufferCreate(GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel);
ShovelerFramebuffer *shovelerFramebufferCreateColorOnly(GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel);
ShovelerFramebuffer *shovelerFramebufferCreateDepthOnly(GLsizei width, GLsizei height, GLsizei samples);
bool shovelerFramebufferUse(ShovelerFramebuffer *framebuffer);
bool shovelerFramebufferBlitToDefault(ShovelerFramebuffer *framebuffer);
void shovelerFramebufferFree(ShovelerFramebuffer *framebuffer);

#endif
