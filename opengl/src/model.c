#include "shoveler/model.h"

#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/opengl.h"
#include "shoveler/shader_cache.h"
#include "shoveler/types.h"
#include "shoveler/uniform.h"

ShovelerModel* shovelerModelCreate(ShovelerDrawable* drawable, ShovelerMaterial* material) {
  ShovelerModel* model = malloc(sizeof(ShovelerModel));
  model->shaderCache = material->shaderCache;
  model->drawable = drawable;
  model->material = material;
  model->translation = shovelerVector3(0, 0, 0);
  model->rotation = shovelerVector3(0, 0, 0);
  model->scale = shovelerVector3(1, 1, 1);
  model->transformation = shovelerMatrixIdentity;
  model->normalTransformation = shovelerMatrixIdentity;
  model->visible = true;
  model->emitter = false;
  model->castsShadow = true;
  model->polygonMode = GL_FILL;
  model->uniforms = shovelerUniformMapCreate();

  ShovelerUniform* modelUniform = shovelerUniformCreateMatrixPointer(&model->transformation);
  shovelerUniformMapInsert(model->uniforms, "model", modelUniform);
  ShovelerUniform* modelNormalUniform =
      shovelerUniformCreateMatrixPointer(&model->normalTransformation);
  shovelerUniformMapInsert(model->uniforms, "modelNormal", modelNormalUniform);

  return model;
}

void shovelerModelUpdateTransformation(ShovelerModel* model) {
  ShovelerMatrix translation = shovelerMatrixIdentity;
  shovelerMatrixGet(translation, 0, 3) = model->translation.values[0];
  shovelerMatrixGet(translation, 1, 3) = model->translation.values[1];
  shovelerMatrixGet(translation, 2, 3) = model->translation.values[2];

  ShovelerMatrix rotationX = shovelerMatrixCreateRotationX(model->rotation.values[0]);
  ShovelerMatrix rotationY = shovelerMatrixCreateRotationY(model->rotation.values[1]);
  ShovelerMatrix rotationZ = shovelerMatrixCreateRotationZ(model->rotation.values[2]);
  ShovelerMatrix rotation =
      shovelerMatrixMultiply(rotationX, shovelerMatrixMultiply(rotationY, rotationZ));

  ShovelerMatrix scale = shovelerMatrixIdentity;
  shovelerMatrixGet(scale, 0, 0) = model->scale.values[0];
  shovelerMatrixGet(scale, 1, 1) = model->scale.values[1];
  shovelerMatrixGet(scale, 2, 2) = model->scale.values[2];

  ShovelerMatrix scaleInverse = shovelerMatrixIdentity;
  shovelerMatrixGet(scaleInverse, 0, 0) = 1.0f / model->scale.values[0];
  shovelerMatrixGet(scaleInverse, 1, 1) = 1.0f / model->scale.values[1];
  shovelerMatrixGet(scaleInverse, 2, 2) = 1.0f / model->scale.values[2];

  model->transformation =
      shovelerMatrixMultiply(translation, shovelerMatrixMultiply(rotation, scale));
  model->normalTransformation = shovelerMatrixMultiply(rotation, scaleInverse);
}

bool shovelerModelRender(ShovelerModel* model) {
  glPolygonMode(GL_FRONT_AND_BACK, model->polygonMode);

  if (!shovelerDrawableDraw(model->drawable)) {
    shovelerLogError("Failed to draw drawable when trying to render model");
    return false;
  }

  return shovelerOpenGLCheckSuccess();
}

void shovelerModelFree(ShovelerModel* model) {
  if (model == NULL) {
    return;
  }

  shovelerShaderCacheInvalidateModel(model->shaderCache, model);

  shovelerUniformMapFree(model->uniforms);
  free(model);
}
