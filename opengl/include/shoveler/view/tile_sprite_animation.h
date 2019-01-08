#ifndef SHOVELER_VIEW_TILEMAP_TILE_SPRITE_ANIMATION_H
#define SHOVELER_VIEW_TILEMAP_TILE_SPRITE_ANIMATION_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/tile_sprite_animation.h>
#include <shoveler/view.h>

typedef struct {
	long long int tileSpriteEntityId;
	float moveAmountThreshold;
} ShovelerViewTileSpriteAnimationConfiguration;

static const char *shovelerViewTileSpriteAnimationComponentName = "tile_sprite_animation";

/** Adds a canvas tile sprite component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration);
ShovelerTileSpriteAnimation *shovelerViewEntityGetTileSpriteAnimation(ShovelerViewEntity *entity);
const ShovelerViewTileSpriteAnimationConfiguration *shovelerViewEntityGetTileSpriteAnimationConfiguration(ShovelerViewEntity *entity);
/** Updates a canvas tile sprite component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration);
bool shovelerViewEntityRemoveTileSpriteAnimation(ShovelerViewEntity *entity);

#endif
