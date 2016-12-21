#include <assert.h> // assert
#include <math.h> // floor, log2, fmax
#include <stdlib.h> // malloc, free

#include "log.h"
#include "opengl.h"
#include "texture.h"

static int getNumMipmapLevels(int width, int height);

ShovelerTexture *shovelerTextureCreate2d(ShovelerImage *image)
{
	assert(image->channels >= 1);
	assert(image->channels <= 4);

	ShovelerTexture *texture = malloc(sizeof(ShovelerTexture));
	texture->image = image;
	texture->target = GL_TEXTURE_2D;
	glGenTextures(1, &texture->texture);
	glBindTexture(texture->target, texture->texture);

	switch(image->channels) {
		case 1:
			texture->internalFormat = GL_R8;
			texture->format = GL_RED;
		break;
		case 2:
			texture->internalFormat = GL_RG8;
			texture->format = GL_RG;
		break;
		case 3:
			texture->internalFormat = GL_RGB8;
			texture->format = GL_RGB;
		break;
		case 4:
			texture->internalFormat = GL_RGBA8;
			texture->format = GL_RGBA;
		break;
	}

	int numMipmapLevels = getNumMipmapLevels(texture->image->width, texture->image->width);
	glTexStorage2D(texture->target, numMipmapLevels, texture->internalFormat, texture->image->width, texture->image->height);

	return texture;
}

ShovelerTexture *shovelerTextureCreateRenderTarget(GLsizei width, GLsizei height)
{
	ShovelerTexture *texture = malloc(sizeof(ShovelerTexture));
	texture->image = NULL;
	texture->target = GL_TEXTURE_2D;
	glGenTextures(1, &texture->texture);
	glBindTexture(texture->target, texture->texture);

	texture->internalFormat = GL_RGBA8;
	texture->format = GL_RGBA;
	glTexStorage2D(texture->target, 1, texture->internalFormat, width, height);

	return texture;
}

ShovelerTexture *shovelerTextureCreateMultisampleRenderTarget(GLsizei width, GLsizei height, GLsizei samples)
{
	ShovelerTexture *texture = malloc(sizeof(ShovelerTexture));
	texture->image = NULL;
	texture->target = GL_TEXTURE_2D_MULTISAMPLE;
	glGenTextures(1, &texture->texture);
	glBindTexture(texture->target, texture->texture);

	texture->internalFormat = GL_RGBA8;
	texture->format = GL_RGBA;
	glTexStorage2DMultisample(texture->target, samples, texture->internalFormat, width, height, false);

	return texture;
}

ShovelerTexture *shovelerTextureCreateDepthTarget(GLsizei width, GLsizei height)
{
	ShovelerTexture *texture = malloc(sizeof(ShovelerTexture));
	texture->image = NULL;
	texture->target = GL_TEXTURE_2D;
	glGenTextures(1, &texture->texture);
	glBindTexture(texture->target, texture->texture);

	texture->internalFormat = GL_DEPTH_COMPONENT32F;
	texture->format = GL_DEPTH_COMPONENT;
	glTexStorage2D(texture->target, 1, texture->internalFormat, width, height);

	return texture;
}

ShovelerTexture *shovelerTextureCreateMultisampleDepthTarget(GLsizei width, GLsizei height, GLsizei samples)
{
	ShovelerTexture *texture = malloc(sizeof(ShovelerTexture));
	texture->image = NULL;
	texture->target = GL_TEXTURE_2D_MULTISAMPLE;
	glGenTextures(1, &texture->texture);
	glBindTexture(texture->target, texture->texture);

	texture->internalFormat = GL_DEPTH_COMPONENT32F;
	texture->format = GL_DEPTH_COMPONENT;
	glTexStorage2DMultisample(texture->target, samples, texture->internalFormat, width, height, false);

	return texture;
}

bool shovelerTextureUpdate(ShovelerTexture *texture)
{
	glBindTexture(texture->target, texture->texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(texture->target, 0, 0, 0, texture->image->width, texture->image->height, texture->format, GL_UNSIGNED_BYTE, texture->image->data);
	glGenerateMipmap(texture->target);
	return shovelerOpenGLCheckSuccess();
}

bool shovelerTextureUse(ShovelerTexture *texture, GLuint unitIndex)
{
	glActiveTexture(GL_TEXTURE0 + unitIndex);
	glBindTexture(texture->target, texture->texture);
	return shovelerOpenGLCheckSuccess();
}

void shovelerTextureFree(ShovelerTexture *texture)
{
	if(texture == NULL) {
		return;
	}

	shovelerImageFree(texture->image);
	glDeleteTextures(1, &texture->texture);
	free(texture);
}

static int getNumMipmapLevels(int width, int height)
{
	return floor(log2(fmax(width, height))) + 1;
}
