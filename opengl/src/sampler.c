#include "shoveler/sampler.h"

#include <stdlib.h> // malloc, free

#include "shoveler/opengl.h"

ShovelerSampler* shovelerSamplerCreate(bool interpolate, bool useMipmaps, bool clamp) {
  ShovelerSampler* sampler = malloc(sizeof(ShovelerSampler));
  glGenSamplers(1, &sampler->sampler);

  if (interpolate) {
    if (useMipmaps) {
      glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
      glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    if (useMipmaps) {
      glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    } else {
      glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  shovelerOpenGLCheckSuccess();

  if (clamp) {
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  shovelerOpenGLCheckSuccess();

  return sampler;
}

bool shovelerSamplerUse(ShovelerSampler* sampler, GLuint unitIndex) {
  glBindSampler(unitIndex, sampler->sampler);
  return shovelerOpenGLCheckSuccess();
}

void shovelerSamplerFree(ShovelerSampler* sampler) {
  glDeleteSamplers(1, &sampler->sampler);
  free(sampler);
}
