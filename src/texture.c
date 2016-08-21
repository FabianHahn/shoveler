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
	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);

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
	glTexStorage2D(GL_TEXTURE_2D, numMipmapLevels, texture->internalFormat, texture->image->width, texture->image->height);

	return texture;
}

bool shovelerTextureUpdate(ShovelerTexture *texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->image->width, texture->image->height, texture->format, GL_UNSIGNED_BYTE, texture->image->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return shovelerOpenGLCheckSuccess();
}

bool shovelerTextureUse(ShovelerTexture *texture, GLuint unitIndex)
{
	glActiveTexture(GL_TEXTURE0 + unitIndex);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	return shovelerOpenGLCheckSuccess();
}

void shovelerTextureFree(ShovelerTexture *texture)
{
	shovelerImageFree(texture->image);
	glDeleteTextures(1, &texture->texture);
	free(texture);
}

static int getNumMipmapLevels(int width, int height)
{
	return floor(log2(fmax(width, height))) + 1;
}
