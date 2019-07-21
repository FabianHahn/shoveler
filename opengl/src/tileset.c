#include <assert.h> // assert
#include <limits.h> // INT_MAX, UINT_MAX
#include <stdlib.h> // malloc free

#include "shoveler/image.h"
#include "shoveler/sampler.h"
#include "shoveler/texture.h"
#include "shoveler/tileset.h"

ShovelerTileset *shovelerTilesetCreate(const ShovelerImage *image, unsigned char columns, unsigned char rows, unsigned char padding)
{
	assert(columns > 0);
	assert(rows > 0);
	assert(image->width % columns == 0);
	assert(image->height % rows == 0);
	assert(image->width <= INT_MAX);
	assert(image->height <= INT_MAX);
	assert(image->width + 2 * padding * columns <= INT_MAX);
	assert(image->height + 2 * padding * rows <= INT_MAX);
	unsigned int tileWidth = image->width / columns;
	unsigned int tileHeight = image->height / rows;
	assert(tileWidth + padding <= INT_MAX);
	assert(tileHeight + padding <= INT_MAX);

	ShovelerTileset *tileset = malloc(sizeof(ShovelerTileset));
	tileset->columns = columns;
	tileset->rows = rows;
	tileset->padding = padding;

	ShovelerImage *paddedImage = shovelerImageCreate(image->width + 2 * padding * columns, image->height + 2 * padding * rows, image->channels);
	for(unsigned char column = 0; column < columns; column++) {
		for(unsigned char row = 0; row < rows; row++) {
			for(int i = -padding; i < (int) tileWidth + padding; i++) {
				int tileI = i;
				if(tileI < 0) {
					tileI = 0;
				} else if(tileI >= (int) tileWidth) {
					tileI = (int) tileWidth - 1;
				}
				assert(tileI >= 0);

				unsigned int imageI = column * tileWidth + tileI;
				assert(imageI < image->width);

				unsigned int paddedImageI = column * (tileWidth + 2 * padding) + padding + i;
				assert(paddedImageI < paddedImage->width);

				for(int j = -padding; j < (int) tileHeight + padding; j++) {
					int tileJ = j;
					if(tileJ < 0) {
						tileJ = 0;
					} else if(tileJ >= (int) tileHeight) {
						tileJ = (int) tileHeight - 1;
					}
					assert(tileJ >= 0);

					unsigned int imageJ = row * tileHeight + tileJ;
					assert(imageJ < image->height);

					unsigned int paddedImageJ = row * (tileHeight + 2 * padding) + padding + j;
					assert(paddedImageJ < paddedImage->height);

					for(unsigned int c = 0; c < image->channels; c++) {
						shovelerImageGet(paddedImage, paddedImageI, paddedImageJ, c) = shovelerImageGet(image, imageI, imageJ, c);
					}
				}
			}
		}
	}

	tileset->manageTexture = true;
	tileset->texture = shovelerTextureCreate2d(paddedImage, true);
	shovelerTextureUpdate(tileset->texture);

	// create a sampler without mipmapping to prevent seam artifacts between tiles
	tileset->sampler = shovelerSamplerCreate(true, false, true);
	return tileset;
}

ShovelerTileset *shovelerTilesetCreateFromTexture(ShovelerTexture *texture, unsigned char columns, unsigned char rows, unsigned char padding)
{
	ShovelerTileset *tileset = malloc(sizeof(ShovelerTileset));
	tileset->columns = columns;
	tileset->rows = rows;
	tileset->padding = padding;
	tileset->manageTexture = false;
	tileset->texture = texture;

	// create a sampler without mipmapping to prevent seam artifacts between tiles
	tileset->sampler = shovelerSamplerCreate(true, false, true);

	return tileset;
}

void shovelerTilesetFree(ShovelerTileset *tileset)
{
	if(tileset == NULL) {
		return;
	}

	shovelerSamplerFree(tileset->sampler);

	if(tileset->manageTexture) {
		shovelerTextureFree(tileset->texture);
	}

	free(tileset);
}
