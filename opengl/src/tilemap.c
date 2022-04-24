#include "shoveler/material/tilemap.h"

#include <stdlib.h> // malloc, free
#include <string.h> // memmove

#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/render_state.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"
#include "shoveler/texture.h"
#include "shoveler/tilemap.h"
#include "shoveler/tileset.h"

ShovelerTilemap* shovelerTilemapCreate(ShovelerTexture* tiles, const bool* collidingTiles) {
  ShovelerTilemap* tilemap = malloc(sizeof(ShovelerTilemap));
  tilemap->tiles = tiles;
  tilemap->tilesets = g_queue_new();
  tilemap->collidingTiles = collidingTiles;

  return tilemap;
}

int shovelerTilemapAddTileset(ShovelerTilemap* tilemap, ShovelerTileset* tileset) {
  g_queue_push_tail(tilemap->tilesets, tileset);
  return g_queue_get_length(tilemap->tilesets); // start with one since zero is blank
}

bool shovelerTilemapIntersect(
    ShovelerTilemap* tilemap,
    const ShovelerBoundingBox2* boundingBox,
    const ShovelerBoundingBox2* object) {
  if (tilemap->collidingTiles == NULL) {
    return false;
  }

  ShovelerVector2 size =
      shovelerVector2LinearCombination(1.0f, boundingBox->max, -1.0f, boundingBox->min);

  unsigned int numColumns = tilemap->tiles->width;
  unsigned int numRows = tilemap->tiles->height;

  float columnStride = size.values[0] / numColumns;
  float rowStride = size.values[1] / numRows;

  for (unsigned int row = 0; row < numRows; row++) {
    unsigned int rowIndex = row * numRows;
    float rowCoordinate = boundingBox->min.values[1] + row * rowStride;

    for (unsigned int column = 0; column < numColumns; column++) {
      if (!tilemap->collidingTiles[rowIndex + column]) {
        continue;
      }

      float columnCoordinate = boundingBox->min.values[0] + column * columnStride;

      ShovelerBoundingBox2 tileBoundingBox = shovelerBoundingBox2(
          shovelerVector2(columnCoordinate, rowCoordinate),
          shovelerVector2(columnCoordinate + columnStride, rowCoordinate + rowStride));
      if (shovelerBoundingBox2Intersect(object, &tileBoundingBox)) {
        return true;
      }
    }
  }

  return false;
}

bool shovelerTilemapRender(
    ShovelerTilemap* tilemap,
    ShovelerVector2 regionPosition,
    ShovelerVector2 regionSize,
    ShovelerMaterial* material,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerRenderState* renderState) {
  // since we are only changing uniform pointer values per per tileset, we can reuse the same shader
  // for all of them
  ShovelerShader* shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

  shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shovelerMaterialTilemapSetActiveRegion(material, regionPosition, regionSize);
  shovelerMaterialTilemapSetActiveTiles(material, tilemap->tiles);

  int tilesetId = 1; // start with one since zero is blank
  for (GList* iter = tilemap->tilesets->head; iter != NULL; iter = iter->next, tilesetId++) {
    ShovelerTileset* tileset = (ShovelerTileset*) iter->data;

    shovelerMaterialTilemapSetActiveTileset(material, tilesetId, tileset);

    if (!shovelerShaderUse(shader)) {
      shovelerLogWarning(
          "Failed to use shader for tileset %p when rendering tilemap %p with material %p and "
          "model %p.",
          tileset,
          tilemap,
          material,
          model);
      return false;
    }

    if (!shovelerModelRender(model)) {
      shovelerLogWarning(
          "Failed to render model %p for tileset %p when rendering tilemap %p with material %p.",
          model,
          tileset,
          tilemap,
          material);
      return false;
    }

    if (!material->screenspace) {
      shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
    }
  }

  return true;
}

void shovelerTilemapFree(ShovelerTilemap* tilemap) {
  if (tilemap == NULL) {
    return;
  }

  g_queue_free(tilemap->tilesets);
  free(tilemap);
}
