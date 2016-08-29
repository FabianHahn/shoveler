#ifndef SHOVELER_TEXTURE_H
#define SHOVELER_TEXTURE_H

#include "image.h"

#include <glad/glad.h>

typedef struct {
	ShovelerImage *image;
	GLuint target;
	GLuint texture;
	GLuint internalFormat;
	GLuint format;
} ShovelerTexture;

ShovelerTexture *shovelerTextureCreate2d(ShovelerImage *image);
ShovelerTexture *shovelerTextureCreateRenderTarget(GLsizei width, GLsizei height);
ShovelerTexture *shovelerTextureCreateMultisampleRenderTarget(GLsizei width, GLsizei height, GLsizei samples);
ShovelerTexture *shovelerTextureCreateDepthTarget(GLsizei width, GLsizei height);
ShovelerTexture *shovelerTextureCreateMultisampleDepthTarget(GLsizei width, GLsizei height, GLsizei samples);
bool shovelerTextureUpdate(ShovelerTexture *texture);
bool shovelerTextureUse(ShovelerTexture *texture, GLuint unitIndex);
void shovelerTextureFree(ShovelerTexture *texture);

#endif
