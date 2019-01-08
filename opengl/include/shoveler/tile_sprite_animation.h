#ifndef SHOVELER_TILE_SPRITE_ANIMATION_H
#define SHOVELER_TILE_SPRITE_ANIMATION_H

#include <shoveler/canvas.h>
#include <shoveler/tileset.h>
#include <shoveler/types.h>

typedef enum {
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_DOWN = 0,
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_UP = 1,
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_LEFT = 2,
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_RIGHT = 3,
} ShovelerTileSpriteAnimationDirection;

typedef struct ShovelerTileSpriteAnimationStruct {
	ShovelerCanvasTileSprite *tileSprite;
	ShovelerTileset *tileset;
	float moveAmountThreshold;
	int numZeroUpdatesForStopping;
	float eps;
	ShovelerTileSpriteAnimationDirection direction;
	unsigned char frame;
	float frameMoveAmount;
	int zeroUpdateCounter;
	bool logDirectionChanges;
} ShovelerTileSpriteAnimation;

ShovelerTileSpriteAnimation *shovelerTileSpriteAnimationCreate(ShovelerCanvasTileSprite *tileSprite, ShovelerTileset *tileset);
void shovelerTileSpriteAnimationUpdate(ShovelerTileSpriteAnimation *animation, ShovelerVector2 moveAmount);
void shovelerTileSpriteAnimationFree(ShovelerTileSpriteAnimation *animation);

#endif
