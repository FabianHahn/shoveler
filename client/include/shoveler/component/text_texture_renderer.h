#ifndef SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_H
#define SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;
typedef struct ShovelerTextTextureRendererStruct ShovelerTextTextureRenderer;

void shovelerClientSystemAddTextTextureRendererSystem(ShovelerClientSystem* clientSystem);

static inline ShovelerTextTextureRenderer* shovelerComponentGetTextTextureRenderer(
    ShovelerComponent* component) {
  assert(component->type->id == shovelerComponentTypeIdTextTextureRenderer);
  return component->systemData;
}

#endif
