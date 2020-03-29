#ifndef SHOVELER_SPRITE_H
#define SHOVELER_SPRITE_H

#include <shoveler/collider.h>
#include <shoveler/types.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerRenderStateStruct ShovelerRenderState; // forward declaration: render_state.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: below

typedef bool (ShovelerSpriteRenderFunction)(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState);
typedef void (ShovelerSpriteFreeFunction)(ShovelerSprite *sprite);

typedef struct ShovelerSpriteStruct {
	ShovelerVector2 position;
	ShovelerVector2 size;
	ShovelerCollider2 collider;
	bool enableCollider;
	ShovelerMaterial *material;
	ShovelerSpriteRenderFunction *render;
	ShovelerSpriteFreeFunction *free;
	void *data;
} ShovelerSprite;

void shovelerSpriteInit(ShovelerSprite *sprite, ShovelerMaterial *material, ShovelerCollider2IntersectFunction *interesect, ShovelerSpriteRenderFunction *render, ShovelerSpriteFreeFunction *free, void *data);
void shovelerSpriteUpdatePosition(ShovelerSprite *sprite, ShovelerVector2 position);

static inline bool shovelerSpriteRender(ShovelerSprite *sprite, ShovelerVector2 regionPosition, ShovelerVector2 regionSize, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerRenderState *renderState)
{
	return sprite->render(sprite, regionPosition, regionSize, scene, camera, light, model, renderState);
}

static inline void shovelerSpriteFree(ShovelerSprite *sprite)
{
	return sprite->free(sprite);
}

#endif
