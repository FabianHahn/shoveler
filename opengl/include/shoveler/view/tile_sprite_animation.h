#ifndef SHOVELER_VIEW_TILE_SPRITE_ANIMATION_H
#define SHOVELER_VIEW_TILE_SPRITE_ANIMATION_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/tile_sprite_animation.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	long long int positionEntityId;
	long long int tileSpriteEntityId;
	ShovelerCoordinateMapping positionMappingX;
	ShovelerCoordinateMapping positionMappingY;
	float moveAmountThreshold;
} ShovelerViewTileSpriteAnimationConfiguration;

static const char *shovelerViewTileSpriteAnimationComponentTypeName = "tile_sprite_animation";
static const char *shovelerViewTileSpriteAnimationPositionOptionKey = "position";
static const char *shovelerViewTileSpriteAnimationTileSpriteOptionKey = "tile_sprite";
static const char *shovelerViewTileSpriteAnimationPositionMappingXOptionKey = "position_mapping_x";
static const char *shovelerViewTileSpriteAnimationPositionMappingYOptionKey = "position_mapping_y";
static const char *shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey = "move_amount_threshold";

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
	return shovelerViewEntityGetComponent(entity, shovelerViewTileSpriteAnimationComponentTypeName);
}

#endif
