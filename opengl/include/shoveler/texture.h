#ifndef SHOVELER_TEXTURE_H
#define SHOVELER_TEXTURE_H

#include <glad/glad.h>
#include <stdbool.h> // bool

typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h

typedef struct ShovelerTextureStruct {
  unsigned int width;
  unsigned int height;
  unsigned int channels;
  ShovelerImage* image;
  bool manageImage;
  GLuint target;
  GLuint texture;
  GLuint internalFormat;
  GLuint format;
} ShovelerTexture;

ShovelerTexture* shovelerTextureCreate2d(ShovelerImage* image, bool manageImage);
ShovelerTexture* shovelerTextureCreateRenderTarget(
    unsigned int width,
    unsigned int height,
    unsigned int channels,
    GLsizei samples,
    int bitsPerChannel);
ShovelerTexture* shovelerTextureCreateDepthTarget(
    unsigned int width, unsigned int height, GLsizei samples);
bool shovelerTextureUpdate(ShovelerTexture* texture);
bool shovelerTextureUse(ShovelerTexture* texture, GLuint unitIndex);
void shovelerTextureFree(ShovelerTexture* texture);

#endif
