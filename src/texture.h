#ifndef SHOVELER_TEXTURE_H
#define SHOVELER_TEXTURE_H

#include "image.h"

#include <glad/glad.h>

typedef struct {
	ShovelerImage *image;
	GLuint texture;
	GLuint internalFormat;
	GLuint format;
} ShovelerTexture;

ShovelerTexture *shovelerTextureCreate2d(ShovelerImage *image);
bool shovelerTextureUpdate(ShovelerTexture *texture);
bool shovelerTextureUse(ShovelerTexture *texture, GLuint unitIndex);
void shovelerTextureFree(ShovelerTexture *texture);

#endif
