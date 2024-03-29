#ifndef SHOVELER_LIGHT_H
#define SHOVELER_LIGHT_H

#include <shoveler/camera.h>
#include <shoveler/filter.h>
#include <shoveler/framebuffer.h>
#include <shoveler/material.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/uniform_map.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

typedef void(ShovelerLightUpdatePositionFunction)(void* data, ShovelerVector3 position);
typedef ShovelerVector3(ShovelerLightGetPositionFunction)(void* data);
typedef int(ShovelerLightRenderFunction)(
    void* data,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerFramebuffer* framebuffer,
    ShovelerSceneRenderPassOptions renderPassOptions,
    ShovelerRenderState* renderState);
typedef void(ShovelerLightFreeDataFunction)(void* data);

typedef struct ShovelerLightStruct {
  struct ShovelerShaderCacheStruct* shaderCache;
  ShovelerUniformMap* uniforms;
  void* data;
  ShovelerLightUpdatePositionFunction* updatePosition;
  ShovelerLightGetPositionFunction* getPosition;
  ShovelerLightRenderFunction* render;
  ShovelerLightFreeDataFunction* freeData;
} ShovelerLight;

static inline void shovelerLightUpdatePosition(ShovelerLight* light, ShovelerVector3 position) {
  light->updatePosition(light->data, position);
}

static inline ShovelerVector3 shovelerLightGetPosition(ShovelerLight* light) {
  return light->getPosition(light->data);
}

static inline int shovelerLightRender(
    ShovelerLight* light,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerFramebuffer* framebuffer,
    ShovelerSceneRenderPassOptions renderPassOptions,
    ShovelerRenderState* renderState) {
  return light->render(light->data, scene, camera, framebuffer, renderPassOptions, renderState);
}

static inline void shovelerLightFree(ShovelerLight* light) { light->freeData(light->data); }

#endif
