#include "shoveler/sprite.h"

void shovelerSpriteInit(ShovelerSprite *sprite, ShovelerMaterial *material, ShovelerSpriteRenderFunction *render, ShovelerSpriteFreeFunction *free, void *data)
{
	sprite->translation = shovelerVector2(0.0f, 0.0f);
	sprite->rotation = shovelerVector2(0.0f, 0.0f);
	sprite->scale = shovelerVector2(1.0f, 1.0f);
	sprite->material = material;
	sprite->render = render;
	sprite->free = free;
	sprite->data = data;
}
