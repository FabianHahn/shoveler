#ifndef SHOVELER_FRAMEBUFFER_H
#define SHOVELER_FRAMEBUFFER_H

#include <glad/glad.h>
#include <shoveler/texture.h>
#include <stdbool.h> // bool

typedef struct ShovelerFramebufferStruct {
  GLuint framebuffer;
  GLsizei width;
  GLsizei height;
  ShovelerTexture* renderTarget;
  ShovelerTexture* depthTarget;
} ShovelerFramebuffer;

ShovelerFramebuffer* shovelerFramebufferCreate(
    GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel);
ShovelerFramebuffer* shovelerFramebufferCreateColorOnly(
    GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel);
ShovelerFramebuffer* shovelerFramebufferCreateDepthOnly(
    GLsizei width, GLsizei height, GLsizei samples);
bool shovelerFramebufferUse(ShovelerFramebuffer* framebuffer);
bool shovelerFramebufferBlitToDefault(ShovelerFramebuffer* framebuffer);
void shovelerFramebufferFree(ShovelerFramebuffer* framebuffer, bool keepTargets);

#endif
