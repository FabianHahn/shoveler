#ifndef SHOVELER_VIEW_TILEMAP_CANVAS_TILE_SPRITE_H
#define SHOVELER_VIEW_TILEMAP_CANVAS_TILE_SPRITE_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/canvas.h>
#include <shoveler/view.h>

typedef struct {
	long long int tilesetEntityId;
	unsigned char tilesetColumn;
	unsigned char tilesetRow;
	ShovelerVector2 position;
	ShovelerVector2 size;
} ShovelerViewCanvasTileSpriteConfiguration;

static const char *shovelerViewCanvasTileSpriteComponentName = "canvas_tile_sprite";

/** Adds a canvas tile sprite component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddCanvasTileSprite(ShovelerViewEntity *entity, const ShovelerViewCanvasTileSpriteConfiguration *configuration);
ShovelerCanvasTileSprite *shovelerViewEntityGetCanvasTileSprite(ShovelerViewEntity *entity);
const ShovelerViewCanvasTileSpriteConfiguration *shovelerViewEntityGetCanvasTileSpriteConfiguration(ShovelerViewEntity *entity);
/** Updates a canvas tile sprite component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateCanvasTileSprite(ShovelerViewEntity *entity, const ShovelerViewCanvasTileSpriteConfiguration *configuration);
bool shovelerViewEntityRemoveCanvasTileSprite(ShovelerViewEntity *entity);

#endif
