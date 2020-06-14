#include <stdlib.h> // malloc free

#include "shoveler/material/tile_sprite.h"
#include "shoveler/sprite/texture.h"
#include "shoveler/sprite/tile.h"
#include "shoveler/material.h"
#include "shoveler/sprite.h"
#include "shoveler/tileset.h"

static bool renderSpriteTexture(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeSpriteTexture(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTextureCreate(ShovelerMaterial *material, ShovelerTexture *texture)
{
	ShovelerSpriteTexture *spriteTexture = malloc(sizeof(ShovelerSpriteTexture));
	shovelerSpriteInit(&spriteTexture->sprite, material, /* intersect */ NULL, renderSpriteTexture, freeSpriteTexture, spriteTexture);
	spriteTexture->tileset = shovelerTilesetCreateFromTexture(texture, /* columns */ 1, /* rows */ 1, /* padding */ 0);

	return &spriteTexture->sprite;
}

static bool renderSpriteTexture(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	ShovelerSpriteTexture *spriteTexture = (ShovelerSpriteTexture *) sprite->data;

	shovelerMaterialTileSpriteSetActiveRegion(sprite->material, regionPosition, regionSize);
	shovelerMaterialTileSpriteSetActiveTile(sprite->material, /* tilesetRow */ 0, /* tilesetColumn */ 0);
	shovelerMaterialTileSpriteSetActiveTileset(sprite->material, spriteTexture->tileset);
	shovelerMaterialTileSpriteSetActiveSprite(sprite->material, sprite->position, sprite->size);

	return shovelerMaterialRender(sprite->material, scene, camera, light, model, renderState);
}

static void freeSpriteTexture(ShovelerSprite *sprite)
{
	ShovelerSpriteTexture *spriteTexture = (ShovelerSpriteTexture *) sprite->data;

	shovelerTilesetFree(spriteTexture->tileset);
	free(spriteTexture);
}
