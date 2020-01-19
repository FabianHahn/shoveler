#include <stdlib.h> // malloc free
#include <string.h> // strdup

#include "shoveler/material/text.h"
#include "shoveler/sprite/text.h"
#include "shoveler/material.h"

static bool renderSpriteText(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
static void freeSpriteText(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTextCreate(ShovelerMaterial *material, ShovelerFontAtlasTexture *fontAtlasTexture, float fontSize, ShovelerVector4 color)
{
	ShovelerSpriteText *spriteText = malloc(sizeof(ShovelerSpriteText));
	shovelerSpriteInit(&spriteText->sprite, material, renderSpriteText, freeSpriteText, spriteText);
	spriteText->fontAtlasTexture = fontAtlasTexture;
	spriteText->content = "";
	spriteText->isContentManaged = false;
	spriteText->fontSize = fontSize;
	spriteText->color = color;

	return &spriteText->sprite;
}

void shovelerSpriteTextSetContent(ShovelerSprite *sprite, const char *content, bool copyContent)
{
	ShovelerSpriteText *spriteText = (ShovelerSpriteText *) sprite->data;

	if(spriteText->isContentManaged) {
		free((void *) spriteText->content);
	}

	if(copyContent) {
		spriteText->content = strdup(content);
	} else {
		spriteText->content = content;
	}
}

static bool renderSpriteText(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	ShovelerSpriteText *spriteText = (ShovelerSpriteText *) sprite->data;

	shovelerMaterialTextSetActiveText(sprite->material, spriteText->content, sprite->translation, spriteText->fontSize);
	shovelerMaterialTextSetActiveColor(sprite->material, spriteText->color);
	shovelerMaterialTextSetActiveFontAtlasTexture(sprite->material, spriteText->fontAtlasTexture);

	return shovelerMaterialRender(sprite->material, scene, camera, light, model, renderState);
}

static void freeSpriteText(ShovelerSprite *sprite)
{
	free(sprite);
}
