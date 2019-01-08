#include <assert.h> // assert
#include <math.h> // fabsf
#include <stdlib.h> // malloc, free

#include "shoveler/log.h"
#include "shoveler/tile_sprite_animation.h"

ShovelerTileSpriteAnimation *shovelerTileSpriteAnimationCreate(ShovelerCanvasTileSprite *tileSprite, float moveAmountThreshold)
{
	assert(tileSprite->tileset->columns >= 4);
	assert(tileSprite->tileset->rows >= 3);

	ShovelerTileSpriteAnimation *animation = malloc(sizeof(ShovelerTileSpriteAnimation));
	animation->tileSprite = tileSprite;
	animation->moveAmountThreshold = moveAmountThreshold;
	animation->numZeroUpdatesForStopping = 2;
	animation->eps = 1.0e-6f;
	animation->direction = SHOVELER_TILE_SPRITE_ANIMATION_STATE_DOWN;
	animation->frame = 0;
	animation->frameMoveAmount = 0.0f;
	animation->zeroUpdateCounter = 0;
	animation->logDirectionChanges = false;

	return animation;
}

void shovelerTileSpriteAnimationUpdate(ShovelerTileSpriteAnimation *animation, ShovelerVector2 moveAmount)
{
	float moveX = moveAmount.values[0];
	float moveY = moveAmount.values[1];
	float moveMagnitudeX = fabsf(moveX);
	float moveMagnitudeY = fabsf(moveY);

	if(animation->frame == 0) { // we are in the neutral frame and might start moving now
		if(moveMagnitudeX < animation->eps && moveMagnitudeY < animation->eps) {
			return; // no movement, just stay in neutral frame
		}

		if(moveMagnitudeX > moveMagnitudeY) { // move on x axis
			if(moveX > 0.0f) { // move right
				animation->direction = SHOVELER_TILE_SPRITE_ANIMATION_STATE_RIGHT;
			} else { // move left
				animation->direction = SHOVELER_TILE_SPRITE_ANIMATION_STATE_LEFT;
			}
			animation->frameMoveAmount += moveMagnitudeX;
		} else { // move on y axis
			if(moveY > 0.0f) { // move up
				animation->direction = SHOVELER_TILE_SPRITE_ANIMATION_STATE_UP;
			} else { // move down
				animation->direction = SHOVELER_TILE_SPRITE_ANIMATION_STATE_DOWN;
			}
			animation->frameMoveAmount += moveMagnitudeY;
		}

		if(animation->logDirectionChanges) {
			shovelerLogInfo("Starting to move into direction %d.", animation->direction, moveX, moveY);
		}

		// transition to moving frame
		animation->frame = 1;
	} else { // we are already moving
		if(moveMagnitudeX < animation->eps && moveMagnitudeY < animation->eps) {
			if(animation->zeroUpdateCounter < animation->numZeroUpdatesForStopping) {
				animation->zeroUpdateCounter++;
			} else {
				animation->frame = 0;
				animation->frameMoveAmount = 0.0f;
				animation->zeroUpdateCounter = 0;

				if (animation->logDirectionChanges) {
					shovelerLogInfo("Stopping to move into direction %d.", animation->direction);
				}
			}
		} else { // we are still moving
			animation->zeroUpdateCounter = 0;

			ShovelerTileSpriteAnimationDirection newDirection;
			if(moveMagnitudeX > moveMagnitudeY) { // move on x axis
				if(moveX > 0.0f) { // move right
					newDirection = SHOVELER_TILE_SPRITE_ANIMATION_STATE_RIGHT;
				} else { // move left
					newDirection = SHOVELER_TILE_SPRITE_ANIMATION_STATE_LEFT;
				}
				animation->frameMoveAmount += moveMagnitudeX;
			} else { // move on y axis
				if(moveY > 0.0f) { // move up
					newDirection = SHOVELER_TILE_SPRITE_ANIMATION_STATE_UP;
				} else { // move down
					newDirection = SHOVELER_TILE_SPRITE_ANIMATION_STATE_DOWN;
				}
				animation->frameMoveAmount += moveMagnitudeY;
			}

			if(animation->frameMoveAmount > animation->moveAmountThreshold) {
				animation->frameMoveAmount -= animation->moveAmountThreshold;
				animation->frame = animation->frame == 1 ? 2 : 1; // switch frame

				if (animation->direction != newDirection) { // we are no longer moving in the same direction
					animation->direction = newDirection;

					if(animation->logDirectionChanges) {
						shovelerLogInfo("Changing move direction to %d while moving.", animation->direction, moveX, moveY);
					}
				}
			}
		}
	}

	animation->tileSprite->tilesetColumn = animation->direction;
	animation->tileSprite->tilesetRow = animation->frame;
}

void shovelerTileSpriteAnimationFree(ShovelerTileSpriteAnimation *animation)
{
	if(animation == NULL) {
		return;
	}

	free(animation);
}
