#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/image.h"
#include "shoveler/tileset.h"

ShovelerTileset *shovelerTilesetCreate(ShovelerImage *image, unsigned char columns, unsigned char rows, unsigned char padding)
{
	assert(image->width % columns == 0);
	assert(image->height % rows == 0);
	int tileWidth = image->width / columns;
	int tileHeight = image->height / rows;

	ShovelerTileset *tileset = malloc(sizeof(ShovelerTileset));
	tileset->columns = columns;
	tileset->rows = rows;
	tileset->padding = padding;

	ShovelerImage *paddedImage = shovelerImageCreate(image->width + 2 * padding * columns, image->height + 2 * padding * rows, image->channels);
	for(unsigned char column = 0; column < columns; column++) {
		for(unsigned char row = 0; row < rows; row++) {
			for(int i = -padding; i < tileWidth + padding; i++) {
				int tileI = i;
				if(tileI < 0) {
					tileI = 0;
				} else if(tileI >= tileWidth) {
					tileI = tileWidth - 1;
				}

				int imageI = column * tileWidth + tileI;
				assert(imageI >= 0);
				assert(imageI < image->width);

				int paddedImageI = column * (tileWidth + 2 * padding) + padding + i;
				assert(paddedImageI >= 0);
				assert(paddedImageI < paddedImage->width);

				for(int j = -padding; j < tileHeight + padding; j++) {
					int tileJ = j;
					if(tileJ < 0) {
						tileJ = 0;
					} else if(tileJ >= tileHeight) {
						tileJ = tileHeight - 1;
					}

					int imageJ = row * tileHeight + tileJ;
					assert(imageJ >= 0);
					assert(imageJ < image->height);

					int paddedImageJ = row * (tileHeight + 2 * padding) + padding + j;
					assert(paddedImageJ >= 0);
					assert(paddedImageJ < paddedImage->height);

					for(int c = 0; c < image->channels; c++) {
						shovelerImageGet(paddedImage, paddedImageI, paddedImageJ, c) = shovelerImageGet(image, imageI, imageJ, c);
					}
				}
			}
		}
	}

	tileset->texture = shovelerTextureCreate2d(paddedImage, true);
	shovelerTextureUpdate(tileset->texture);

	tileset->sampler = shovelerSamplerCreate(true, true);
	return tileset;
}

void shovelerTilesetFree(ShovelerTileset *tileset)
{
	if(tileset == NULL) {
		return;
	}

	shovelerSamplerFree(tileset->sampler);
	shovelerTextureFree(tileset->texture);
	free(tileset);
}
