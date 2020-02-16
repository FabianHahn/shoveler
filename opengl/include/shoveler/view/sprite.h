#ifndef SHOVELER_VIEW_SPRITE_H
#define SHOVELER_VIEW_SPRITE_H

#include <stdbool.h> // bool

#include <shoveler/component/sprite.h>
#include <shoveler/canvas.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef struct {
	long long int positionEntityId;
	ShovelerCoordinateMapping positionMappingX;
	ShovelerCoordinateMapping positionMappingY;
	long long int canvasEntityId;
	int layer;
	ShovelerVector2 size;
	long long int tileSpriteEntityId;
} ShovelerViewSpriteConfiguration;

/** Adds a sprite component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddSprite(ShovelerViewEntity *entity, const ShovelerViewSpriteConfiguration *configuration);
ShovelerSprite *shovelerViewEntityGetSprite(ShovelerViewEntity *entity);
/** Returns the current sprite configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetSpriteConfiguration(ShovelerViewEntity *entity, ShovelerViewSpriteConfiguration *outputConfiguration);
/** Updates a sprite component on an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateSprite(ShovelerViewEntity *entity, const ShovelerViewSpriteConfiguration *configuration);
bool shovelerViewEntityRemoveSprite(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetSpriteComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdSprite);
}

#endif
