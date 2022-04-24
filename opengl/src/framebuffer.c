#include "shoveler/framebuffer.h"

#include <stdlib.h> // malloc, free

#include "shoveler/log.h"
#include "shoveler/opengl.h"

static void handleFramebufferIncomplete(GLenum error);

ShovelerFramebuffer* shovelerFramebufferCreate(
    GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel) {
  ShovelerFramebuffer* framebuffer = malloc(sizeof(ShovelerFramebuffer));
  glGenFramebuffers(1, &framebuffer->framebuffer);
  framebuffer->width = width;
  framebuffer->height = height;
  framebuffer->renderTarget =
      shovelerTextureCreateRenderTarget(width, height, channels, samples, bitsPerChannel);
  framebuffer->depthTarget = shovelerTextureCreateDepthTarget(width, height, samples);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0,
      framebuffer->renderTarget->target,
      framebuffer->renderTarget->texture,
      0);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      framebuffer->depthTarget->target,
      framebuffer->depthTarget->texture,
      0);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    handleFramebufferIncomplete(status);
  }

  return framebuffer;
}

ShovelerFramebuffer* shovelerFramebufferCreateColorOnly(
    GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel) {
  ShovelerFramebuffer* framebuffer = malloc(sizeof(ShovelerFramebuffer));
  glGenFramebuffers(1, &framebuffer->framebuffer);
  framebuffer->width = width;
  framebuffer->height = height;
  framebuffer->renderTarget =
      shovelerTextureCreateRenderTarget(width, height, channels, samples, bitsPerChannel);
  framebuffer->depthTarget = NULL;

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0,
      framebuffer->renderTarget->target,
      framebuffer->renderTarget->texture,
      0);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    handleFramebufferIncomplete(status);
  }

  return framebuffer;
}

ShovelerFramebuffer* shovelerFramebufferCreateDepthOnly(
    GLsizei width, GLsizei height, GLsizei samples) {
  ShovelerFramebuffer* framebuffer = malloc(sizeof(ShovelerFramebuffer));
  glGenFramebuffers(1, &framebuffer->framebuffer);
  framebuffer->width = width;
  framebuffer->height = height;
  framebuffer->renderTarget = NULL;
  framebuffer->depthTarget = shovelerTextureCreateDepthTarget(width, height, samples);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      framebuffer->depthTarget->target,
      framebuffer->depthTarget->texture,
      0);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    handleFramebufferIncomplete(status);
  }

  return framebuffer;
}

bool shovelerFramebufferUse(ShovelerFramebuffer* framebuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);

  if (framebuffer->renderTarget != NULL) {
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
  } else {
    glDrawBuffer(GL_NONE);
  }

  glViewport(0, 0, framebuffer->width, framebuffer->height);
  return shovelerOpenGLCheckSuccess();
}

bool shovelerFramebufferBlitToDefault(ShovelerFramebuffer* framebuffer) {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->framebuffer);
  glDrawBuffer(GL_BACK);
  glBlitFramebuffer(
      0,
      0,
      framebuffer->width,
      framebuffer->height,
      0,
      0,
      framebuffer->width,
      framebuffer->height,
      GL_COLOR_BUFFER_BIT,
      GL_NEAREST);
  return shovelerOpenGLCheckSuccess();
}

void shovelerFramebufferFree(ShovelerFramebuffer* framebuffer, bool keepTargets) {
  if (framebuffer == NULL) {
    return;
  }

  if (!keepTargets) {
    shovelerTextureFree(framebuffer->depthTarget);
    shovelerTextureFree(framebuffer->renderTarget);
  }

  glDeleteFramebuffers(1, &framebuffer->framebuffer);
  free(framebuffer);
}

static void handleFramebufferIncomplete(GLenum error) {
  switch (error) {
  case GL_FRAMEBUFFER_UNDEFINED:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_UNDEFINED: The specified framebuffer is "
        "the default read or draw framebuffer, but the default framebuffer does not exist.");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: The framebuffer "
        "does not have at least one image attached to it.");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The "
        "framebuffer does not have at least one image attached to it.");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The value of "
        "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named "
        "by GL_DRAW_BUFFERi.");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: GL_READ_BUFFER "
        "is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the "
        "color attachment point named by GL_READ_BUFFER.");
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_UNSUPPORTED: The combination of "
        "internal formats of the attached images violates an implementation-dependent set of "
        "restrictions.");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The value of "
        "GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; or, the value of "
        "GL_TEXTURE_SAMPLES is the not same for all attached textures; or, the attached images are "
        "a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match "
        "the value of GL_TEXTURE_SAMPLES; or, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is "
        "not the same for all attached textures; or, the attached images are a mix of "
        "renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE "
        "for all attached textures.");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
    shovelerLogError(
        "OpenGL framebuffer incomplete with GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: A framebuffer "
        "attachment is layered, and any populated attachment is not layered, or all populated "
        "color attachments are not from textures of the same target.");
    break;
  default:
    shovelerLogError("unknown OpenGL framebuffer status %d", error);
    break;
  }
}
