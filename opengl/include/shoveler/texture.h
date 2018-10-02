#ifndef SHOVELER_TEXTURE_H
#define SHOVELER_TEXTURE_H

#include <stdbool.h> // bool

#include <shoveler/image.h>

#include <glad/glad.h>

typedef struct {
	ShovelerImage *image;
	bool manageImage;
	GLuint target;
	GLuint texture;
	GLuint internalFormat;
	GLuint format;
} ShovelerTexture;

ShovelerTexture *shovelerTextureCreate2d(ShovelerImage *image, bool manageImage);
ShovelerTexture *shovelerTextureCreateRenderTarget(GLsizei width, GLsizei height, GLsizei samples, int channels, int bitsPerChannel);
ShovelerTexture *shovelerTextureCreateDepthTarget(GLsizei width, GLsizei height, GLsizei samples);
bool shovelerTextureUpdate(ShovelerTexture *texture);
bool shovelerTextureUse(ShovelerTexture *texture, GLuint unitIndex);
void shovelerTextureFree(ShovelerTexture *texture);

#endif
