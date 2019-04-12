#ifndef SHOVELER_TILEMAP_H
#define SHOVELER_TILEMAP_H

#include <glib.h>

#include <shoveler/types.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h
struct ShovelerRenderStateStruct; // forward declaration: render_state.h
struct ShovelerSceneStruct; // forward declaration: scene.h
struct ShovelerTextureStruct; // forward declaration: texture.h
struct ShovelerTilesetStruct; // forward declaration: tileset.h

typedef struct ShovelerTilemapStruct {
	struct ShovelerTextureStruct *tiles;
	/** list of (ShovelerTileset *) */
	GQueue *tilesets;
	/**
	 * Array of booleans indicating colliding tiles, where tile (column, row) is at position [row * numColumns + column].
	 */
	const bool *collidingTiles;
} ShovelerTilemap;

/** Creates a tilemap from a texture and an array of colliding tiles, with the caller retaining ownership over both. */
ShovelerTilemap *shovelerTilemapCreate(struct ShovelerTextureStruct *tiles, const bool *collidingTiles);
/** Adds a tileset to the tilemap, returning its index. */
int shovelerTilemapAddTileset(ShovelerTilemap *tilemap, struct ShovelerTilesetStruct *tileset);
bool shovelerTilemapIntersect(ShovelerTilemap *tilemap, const ShovelerBoundingBox2 *boundingBox, const ShovelerBoundingBox2 *object);
bool shovelerTilemapRender(ShovelerTilemap *tilemap, struct ShovelerMaterialStruct *material, struct ShovelerSceneStruct *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, struct ShovelerRenderStateStruct *renderState);
void shovelerTilemapFree(ShovelerTilemap *tilemap);

#endif
