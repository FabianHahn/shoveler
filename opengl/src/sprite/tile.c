#include <stdlib.h> // malloc free

#include "shoveler/material/tile_sprite.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/material.h"
#include "shoveler/sprite.h"

static bool renderSpriteTile(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material, ShovelerRenderState *renderState);
static void freeSpriteTile(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTileCreate(ShovelerTileset *tileset, int tilesetRow, int tilesetColumn)
{
	ShovelerSpriteTile *spriteTile = malloc(sizeof(ShovelerSpriteTile));
	shovelerSpriteInit(&spriteTile->sprite);
	spriteTile->tileset = tileset;
	spriteTile->tilesetRow = tilesetRow;
	spriteTile->tilesetColumn = tilesetColumn;

	return &spriteTile->sprite;
}

static bool renderSpriteTile(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material, ShovelerRenderState *renderState)
{
	ShovelerSpriteTile *spriteTile = (ShovelerSpriteTile *) sprite->data;

	shovelerMaterialTileSpriteSetActive(material, spriteTile);

	return shovelerMaterialRender(material, scene, camera, light, model, renderState);
}

static void freeSpriteTile(ShovelerSprite *sprite)
{
	free(sprite);
}
