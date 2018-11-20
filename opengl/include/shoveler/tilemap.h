#ifndef SHOVELER_TILEMAP_H
#define SHOVELER_TILEMAP_H

#include <glib.h>

#include <shoveler/scene.h>
#include <shoveler/texture.h>
#include <shoveler/tileset.h>

struct ShovelerCameraStruct; // forward declaration: camera.h
struct ShovelerLightStruct; // forward declaration: light.h
struct ShovelerMaterialStruct; // forward declaration: material.h
struct ShovelerModelStruct; // forward declaration: model.h

typedef struct {
	ShovelerTexture *tiles;
	/** list of (ShovelerTileset *) */
	GQueue *tilesets;
} ShovelerTilemap;

ShovelerTilemap *shovelerTilemapCreate(ShovelerTexture *tiles);
/** Adds a tileset to the tilemap, returning its index. */
int shovelerTilemapAddTileset(ShovelerTilemap *tilemap, ShovelerTileset *tileset);
bool shovelerTilemapRender(ShovelerTilemap *tilemap, struct ShovelerMaterialStruct *material, ShovelerScene *scene, struct ShovelerCameraStruct *camera, struct ShovelerLightStruct *light, struct ShovelerModelStruct *model, ShovelerSceneRenderPassOptions *options);
void shovelerTilemapFree(ShovelerTilemap *tilemap);

#endif
