#ifndef SHOVELER_TILEMAP_H
#define SHOVELER_TILEMAP_H

#include <glib.h>
#include <shoveler/types.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerRenderStateStruct ShovelerRenderState; // forward declaration: render_state.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerTextureStruct ShovelerTexture; // forward declaration: texture.h
typedef struct ShovelerTilesetStruct ShovelerTileset; // forward declaration: tileset.h

typedef struct ShovelerTilemapStruct {
  ShovelerTexture* tiles;
  /** list of (ShovelerTileset *) */
  GQueue* tilesets;
  /**
   * Array of booleans indicating colliding tiles, where tile (column, row) is at position [row *
   * numColumns + column].
   */
  const bool* collidingTiles;
} ShovelerTilemap;

/** Creates a tilemap from a texture and an array of colliding tiles, with the caller retaining
 * ownership over both. */
ShovelerTilemap* shovelerTilemapCreate(ShovelerTexture* tiles, const bool* collidingTiles);
/** Adds a tileset to the tilemap, returning its index. */
int shovelerTilemapAddTileset(ShovelerTilemap* tilemap, ShovelerTileset* tileset);
bool shovelerTilemapIntersect(
    ShovelerTilemap* tilemap,
    const ShovelerBoundingBox2* boundingBox,
    const ShovelerBoundingBox2* object);
bool shovelerTilemapRender(
    ShovelerTilemap* tilemap,
    ShovelerVector2 regionPosition,
    ShovelerVector2 regionSize,
    ShovelerMaterial* material,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerRenderState* renderState);
void shovelerTilemapFree(ShovelerTilemap* tilemap);

#endif
