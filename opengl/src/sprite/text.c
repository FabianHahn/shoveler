#include <stdlib.h> // malloc free
#include <string.h> // strdup

#include "shoveler/material/text.h"
#include "shoveler/sprite/text.h"
#include "shoveler/material.h"

static bool renderSpriteTile(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material, ShovelerRenderState *renderState);
static void freeSpriteTile(ShovelerSprite *sprite);

ShovelerSprite *shovelerSpriteTextCreate(ShovelerFontAtlasTexture *fontAtlasTexture, float fontSize, ShovelerVector4 color)
{
	ShovelerSpriteText *spriteText = malloc(sizeof(ShovelerSpriteText));
	shovelerSpriteInit(&spriteText->sprite);
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

static bool renderSpriteTile(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material, ShovelerRenderState *renderState)
{
	ShovelerSpriteText *spriteText = (ShovelerSpriteText *) sprite->data;

	shovelerMaterialTextSetActiveText(material, spriteText->content, sprite->translation, spriteText->fontSize);
	shovelerMaterialTextSetActiveColor(material, spriteText->color);
	shovelerMaterialTextSetActiveFontAtlasTexture(material, spriteText->fontAtlasTexture);

	return shovelerMaterialRender(material, scene, camera, light, model, renderState);
}

static void freeSpriteTile(ShovelerSprite *sprite)
{
	free(sprite);
}
