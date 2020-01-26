#include <stdlib.h> // malloc free

#include "shoveler/material/tilemap.h"
#include "shoveler/material.h"
#include "shoveler/sprite/tilemap.h"
#include "shoveler/sprite.h"
#include "shoveler/tilemap.h"

static bool renderSpriteTilemap(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeSpriteTilemap(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTilemapCreate(ShovelerMaterial *material, ShovelerTilemap *tilemap)
{
	ShovelerSpriteTilemap *spriteTilemap = malloc(sizeof(ShovelerSpriteTilemap));
	shovelerSpriteInit(&spriteTilemap->sprite, material, renderSpriteTilemap, freeSpriteTilemap, spriteTilemap);
	spriteTilemap->tilemap = tilemap;

	return &spriteTilemap->sprite;
}

static bool renderSpriteTilemap(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	ShovelerSpriteTilemap *spriteTilemap = (ShovelerSpriteTilemap *) sprite->data;

	shovelerMaterialTilemapSetActiveRegion(sprite->material, regionPosition, regionSize);
	shovelerMaterialTilemapSetActiveSprite(sprite->material, spriteTilemap);

	return shovelerMaterialRender(sprite->material, scene, camera, light, model, renderState);
}

static void freeSpriteTilemap(ShovelerSprite *sprite)
{
	free(sprite);
}
