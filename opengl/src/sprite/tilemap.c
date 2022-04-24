#include "shoveler/material/tilemap.h"

#include <stdlib.h> // malloc free

#include "shoveler/material.h"
#include "shoveler/sprite.h"
#include "shoveler/sprite/tilemap.h"
#include "shoveler/tilemap.h"

static const ShovelerCollider2* intersectTilemapSpriteCollider(
    const ShovelerCollider2* collider,
    const ShovelerBoundingBox2* object,
    ShovelerCollider2FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData);
static bool renderTilemapSprite(
    ShovelerSprite* sprite,
    ShovelerVector2 regionPosition,
    ShovelerVector2 regionSize,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerRenderState* renderState);
static void freeTilemapSprite(ShovelerSprite* sprite);

ShovelerSprite* shovelerSpriteTilemapCreate(ShovelerMaterial* material, ShovelerTilemap* tilemap) {
  ShovelerSpriteTilemap* tilemapSprite = malloc(sizeof(ShovelerSpriteTilemap));
  shovelerSpriteInit(
      &tilemapSprite->sprite,
      material,
      intersectTilemapSpriteCollider,
      renderTilemapSprite,
      freeTilemapSprite,
      tilemapSprite);
  tilemapSprite->tilemap = tilemap;

  return &tilemapSprite->sprite;
}

static const ShovelerCollider2* intersectTilemapSpriteCollider(
    const ShovelerCollider2* collider,
    const ShovelerBoundingBox2* object,
    ShovelerCollider2FilterCandidateFunction* filterCandidate,
    void* filterCandidateUserData) {
  ShovelerSpriteTilemap* tilemapSprite = collider->data;

  if (shovelerTilemapIntersect(tilemapSprite->tilemap, &collider->boundingBox, object)) {
    return collider;
  }

  return NULL;
}

static bool renderTilemapSprite(
    ShovelerSprite* sprite,
    ShovelerVector2 regionPosition,
    ShovelerVector2 regionSize,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerRenderState* renderState) {
  ShovelerSpriteTilemap* tilemapSprite = (ShovelerSpriteTilemap*) sprite->data;

  shovelerMaterialTilemapSetActiveRegion(sprite->material, regionPosition, regionSize);
  shovelerMaterialTilemapSetActiveSprite(sprite->material, tilemapSprite);

  return shovelerMaterialRender(sprite->material, scene, camera, light, model, renderState);
}

static void freeTilemapSprite(ShovelerSprite* sprite) {
  ShovelerSpriteTilemap* tilemapSprite = (ShovelerSpriteTilemap*) sprite->data;

  free(tilemapSprite);
}
