#include "shoveler/component/model.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component_system.h"
#include "shoveler/model.h"
#include "shoveler/component/position.h"
#include "shoveler/component/drawable.h"
#include "shoveler/component/material.h"
#include "shoveler/scene.h"
#include "shoveler/schema.h"
#include "shoveler/shader_cache.h"
#include "shoveler/system.h"

static void* activateModelComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateModelComponent(ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdateModelPositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer);
static GLuint convertPolygonMode(ShovelerComponentModelPolygonMode polygonMode);

void shovelerClientSystemAddModelSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdModel);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateModelComponent;
  componentSystem->deactivateComponent = deactivateModelComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION]
      .liveUpdateDependencyField = liveUpdateModelPositionDependency;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateModelComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  ShovelerComponent* drawableComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_FIELD_ID_DRAWABLE);
  assert(drawableComponent != NULL);
  ShovelerDrawable* drawable = shovelerComponentGetDrawable(drawableComponent);
  assert(drawable != NULL);

  ShovelerComponent* materialComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_FIELD_ID_MATERIAL);
  assert(materialComponent != NULL);
  ShovelerMaterial* material = shovelerComponentGetMaterial(materialComponent);
  assert(material != NULL);

  ShovelerModel* model = shovelerModelCreate(drawable, material);
  model->translation = *position;
  model->rotation = shovelerComponentGetFieldValueVector3(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_ROTATION);
  model->scale = shovelerComponentGetFieldValueVector3(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_SCALE);
  model->visible = shovelerComponentGetFieldValueBool(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_VISIBLE);
  model->emitter = shovelerComponentGetFieldValueBool(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_EMITTER);
  model->castsShadow = shovelerComponentGetFieldValueBool(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_CASTS_SHADOW);
  model->polygonMode = convertPolygonMode(shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_MODEL_FIELD_ID_POLYGON_MODE));
  shovelerModelUpdateTransformation(model);

  shovelerSceneAddModel(clientSystem->scene, model);

  return model;
}

static void deactivateModelComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;
  ShovelerModel* model = (ShovelerModel*) component->systemData;

  shovelerShaderCacheInvalidateModel(clientSystem->shaderCache, model);
  shovelerSceneRemoveModel(clientSystem->scene, model);
}

static bool liveUpdateModelPositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer) {
  ShovelerModel* model = (ShovelerModel*) component->systemData;

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_FIELD_ID_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  model->translation = *position;
  shovelerModelUpdateTransformation(model);

  return false; // don't propagate
}

static GLuint convertPolygonMode(ShovelerComponentModelPolygonMode polygonMode) {
  switch (polygonMode) {
  case SHOVELER_COMPONENT_MODEL_POLYGON_MODE_POINT:
    return GL_POINT;
  case SHOVELER_COMPONENT_MODEL_POLYGON_MODE_LINE:
    return GL_LINE;
  case SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL:
  default:
    return GL_FILL;
  }
}
