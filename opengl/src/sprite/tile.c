#include <stdlib.h> // malloc free

#include "shoveler/material/tile_sprite.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/material.h"
#include "shoveler/sprite.h"

static bool renderSpriteTile(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeSpriteTile(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTileCreate(ShovelerMaterial *material, ShovelerTileset *tileset, int tilesetRow, int tilesetColumn)
{
	ShovelerSpriteTile *spriteTile = malloc(sizeof(ShovelerSpriteTile));
	shovelerSpriteInit(&spriteTile->sprite, material, renderSpriteTile, freeSpriteTile, spriteTile);
	spriteTile->tileset = tileset;
	spriteTile->tilesetRow = tilesetRow;
	spriteTile->tilesetColumn = tilesetColumn;

	return &spriteTile->sprite;
}

ShovelerTileset *shovelerSpriteTileGetTileset(ShovelerSprite *sprite)
{
	ShovelerSpriteTile *spriteTile = (ShovelerSpriteTile *) sprite->data;

	return spriteTile->tileset;
}

ShovelerTileset *shovelerSpriteTileSetIndices(ShovelerSprite *sprite, int tilesetRow, int tilesetColumn)
{
	ShovelerSpriteTile *spriteTile = (ShovelerSpriteTile *) sprite->data;

	spriteTile->tilesetRow = tilesetRow;
	spriteTile->tilesetColumn = tilesetColumn;
}

static bool renderSpriteTile(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	ShovelerSpriteTile *spriteTile = (ShovelerSpriteTile *) sprite->data;

	shovelerMaterialTileSpriteSetActive(sprite->material, spriteTile);

	return shovelerMaterialRender(sprite->material, scene, camera, light, model, renderState);
}

static void freeSpriteTile(ShovelerSprite *sprite)
{
	free(sprite);
}
