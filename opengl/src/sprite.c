#include "shoveler/sprite.h"

void shovelerSpriteInit(ShovelerSprite *sprite, ShovelerMaterial *material, ShovelerCollider2IntersectFunction *intersect, ShovelerSpriteRenderFunction *render, ShovelerSpriteFreeFunction *free, void *data)
{
	sprite->position = shovelerVector2(0.0f, 0.0f);
	sprite->size = shovelerVector2(1.0f, 1.0f);
	sprite->collider.boundingBox = shovelerBoundingBox2(
		shovelerVector2LinearCombination(1.0f, sprite->position, -0.5f, sprite->size),
		shovelerVector2LinearCombination(1.0f, sprite->position, 0.5f, sprite->size));
	sprite->collider.intersect = intersect;
	sprite->collider.data = data;
	sprite->enableCollider = true;
	sprite->material = material;
	sprite->render = render;
	sprite->free = free;
	sprite->data = data;
}

void shovelerSpriteUpdatePosition(ShovelerSprite *sprite, ShovelerVector2 position)
{
	sprite->position = position;
	sprite->collider.boundingBox = shovelerBoundingBox2(
		shovelerVector2LinearCombination(1.0f, sprite->position, -0.5f, sprite->size),
		shovelerVector2LinearCombination(1.0f, sprite->position, 0.5f, sprite->size));
}
