#include <stdlib.h> // malloc, free

#include "shoveler/material/tilemap.h"
#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/tilemap.h"

ShovelerTilemap *shovelerTilemapCreate(ShovelerTexture *tiles)
{
	ShovelerTilemap *tilemap = malloc(sizeof(ShovelerTilemap));
	tilemap->tiles = tiles;
	tilemap->tilesets = g_queue_new();
}

int shovelerTilemapAddTileset(ShovelerTilemap *tilemap, ShovelerTileset *tileset)
{
	g_queue_push_tail(tilemap->tilesets, tileset);
	return g_queue_get_length(tilemap->tilesets); // start with one since zero is blank
}

bool shovelerTilemapRender(ShovelerTilemap *tilemap, ShovelerMaterial *material, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	// since we are only changing uniform pointer values per per tileset, we can reuse the same shader for all of them
	ShovelerShader *shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

	shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shovelerMaterialTilemapSetActiveTiles(material, tilemap->tiles);

	int tilesetId = 1; // start with one since zero is blank
	for(GList *iter = tilemap->tilesets->head; iter != NULL; iter = iter->next, tilesetId++) {
		ShovelerTileset *tileset = (ShovelerTileset *) iter->data;

		shovelerMaterialTilemapSetActiveTileset(material, tilesetId, tileset);

		if(!shovelerShaderUse(shader)) {
			shovelerLogWarning("Failed to use shader for tileset %p when rendering tilemap %p with material %p and model %p.", tileset, tilemap, material, model);
			return false;
		}

		if(!shovelerModelRender(model)) {
			shovelerLogWarning("Failed to render model %p for tileset %p when rendering tilemap %p with material %p.", model, tileset, tilemap, material);
			return false;
		}

		shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
	}

	return true;
}

void shovelerTilemapFree(ShovelerTilemap *tilemap)
{
	if(tilemap == NULL) {
		return;
	}

	g_queue_free(tilemap->tilesets);
	free(tilemap);
}
