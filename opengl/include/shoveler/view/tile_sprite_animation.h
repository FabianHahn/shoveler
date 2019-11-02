#ifndef SHOVELER_VIEW_TILE_SPRITE_ANIMATION_H
#define SHOVELER_VIEW_TILE_SPRITE_ANIMATION_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/component/tile_sprite_animation.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	long long int positionEntityId;
	long long int tileSpriteEntityId;
	ShovelerCoordinateMapping positionMappingX;
	ShovelerCoordinateMapping positionMappingY;
	float moveAmountThreshold;
} ShovelerViewTileSpriteAnimationConfiguration;

/** Adds a canvas tile sprite animation component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration);
ShovelerTileSpriteAnimation *shovelerViewEntityGetTileSpriteAnimation(ShovelerViewEntity *entity);
/** Returns the current tile sprite animation configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetTileSpriteAnimationConfiguration(ShovelerViewEntity *entity, ShovelerViewTileSpriteAnimationConfiguration *outputConfiguration);
/** Updates a canvas tile sprite animation component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateTileSpriteAnimation(ShovelerViewEntity *entity, const ShovelerViewTileSpriteAnimationConfiguration *configuration);
bool shovelerViewEntityRemoveTileSpriteAnimation(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetTileSpriteAnimationComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdTileSpriteAnimation);
}

#endif
