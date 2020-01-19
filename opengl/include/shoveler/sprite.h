#ifndef SHOVELER_SPRITE_H
#define SHOVELER_SPRITE_H

#include <shoveler/types.h>

typedef struct ShovelerCameraStruct ShovelerCamera; // forward declaration: camera.h
typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerRenderStateStruct ShovelerRenderState; // forward declaration: render_state.h
typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerSpriteStruct ShovelerSprite; // forward declaration: below

typedef bool (ShovelerSpriteRenderFunction)(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material, ShovelerRenderState *renderState);
typedef void (ShovelerSpriteFreeFunction)(ShovelerSprite *sprite);

typedef struct ShovelerSpriteStruct {
	ShovelerVector2 translation;
	ShovelerVector2 rotation;
	ShovelerVector2 scale;
	ShovelerSpriteRenderFunction *render;
	ShovelerSpriteFreeFunction *free;
	void *data;
} ShovelerSprite;

void shovelerSpriteInit(ShovelerSprite *sprite);

static inline bool shovelerSpriteRender(ShovelerSprite *sprite, ShovelerScene *scene, ShovelerCamera *camera, ShovelerLight *light, ShovelerModel *model, ShovelerMaterial *material, ShovelerRenderState *renderState)
{
	return sprite->render(sprite, scene, camera, light, model, material, renderState);
}

static inline void shovelerSpriteFree(ShovelerSprite *sprite)
{
	return sprite->free(sprite);
}

#endif
