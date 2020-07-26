#include <stdlib.h> // malloc free

#include "shoveler/sprite/texture.h"
#include "shoveler/material/texture_sprite.h"
#include "shoveler/material.h"
#include "shoveler/sprite.h"

static bool renderSpriteTexture(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeSpriteTexture(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTextureCreate(ShovelerMaterial *material, ShovelerTexture *texture, ShovelerSampler *sampler)
{
	ShovelerSpriteTexture *spriteTexture = malloc(sizeof(ShovelerSpriteTexture));
	shovelerSpriteInit(&spriteTexture->sprite, material, /* intersect */ NULL, renderSpriteTexture, freeSpriteTexture, spriteTexture);
	spriteTexture->texture = texture;
	spriteTexture->sampler = sampler;

	return &spriteTexture->sprite;
}

static bool renderSpriteTexture(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	ShovelerSpriteTexture *spriteTexture = (ShovelerSpriteTexture *) sprite->data;

	shovelerMaterialTextureSpriteSetActiveRegion(sprite->material, regionPosition, regionSize);
	shovelerMaterialTextureSpriteSetActiveSprite(sprite->material, sprite->position, sprite->size);
	shovelerMaterialTextureSpriteSetActiveTexture(sprite->material, spriteTexture->texture, spriteTexture->sampler);

	return shovelerMaterialRender(sprite->material, scene, camera, light, model, renderState);
}

static void freeSpriteTexture(ShovelerSprite *sprite)
{
	ShovelerSpriteTexture *spriteTexture = (ShovelerSpriteTexture *) sprite->data;

	free(spriteTexture);
}
