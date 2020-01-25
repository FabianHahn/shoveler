#ifndef SHOVELER_TILE_SPRITE_ANIMATION_H
#define SHOVELER_TILE_SPRITE_ANIMATION_H

#include <shoveler/tileset.h>
#include <shoveler/types.h>

typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: sprite.h

typedef enum {
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_DOWN = 0,
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_UP = 1,
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_LEFT = 2,
	SHOVELER_TILE_SPRITE_ANIMATION_STATE_RIGHT = 3,
} ShovelerTileSpriteAnimationDirection;

typedef struct ShovelerTileSpriteAnimationStruct {
	ShovelerSprite *tileSprite;
	ShovelerVector2 currentPosition;
	float moveAmountThreshold;
	int numZeroUpdatesForStopping;
	float eps;
	ShovelerTileSpriteAnimationDirection direction;
	unsigned char frame;
	float frameMoveAmount;
	int zeroUpdateCounter;
	bool logDirectionChanges;
} ShovelerTileSpriteAnimation;

ShovelerTileSpriteAnimation *shovelerTileSpriteAnimationCreate(ShovelerSprite *tileSprite, ShovelerVector2 initialPosition, float moveAmountThreshold);
void shovelerTileSpriteAnimationUpdate(ShovelerTileSpriteAnimation *animation, ShovelerVector2 newPosition);
void shovelerTileSpriteAnimationFree(ShovelerTileSpriteAnimation *animation);

#endif
