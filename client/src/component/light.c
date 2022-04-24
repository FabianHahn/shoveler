#include "shoveler/component/light.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/position.h"
#include "shoveler/component_system.h"
#include "shoveler/light.h"
#include "shoveler/light/point.h"
#include "shoveler/light/spot.h"
#include "shoveler/log.h"
#include "shoveler/scene.h"
#include "shoveler/schema.h"
#include "shoveler/shader_cache.h"
#include "shoveler/system.h"

static void* activateLightComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateLightComponent(ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdateLightPositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer);

void shovelerClientSystemAddLightSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdLight);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateLightComponent;
  componentSystem->deactivateComponent = deactivateLightComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_LIGHT_FIELD_ID_POSITION]
      .liveUpdateDependencyField = liveUpdateLightPositionDependency;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateLightComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  ShovelerComponentLightType type =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_TYPE);
  ShovelerLight* light;
  switch (type) {
  case SHOVELER_COMPONENT_LIGHT_TYPE_SPOT:
    shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
    return NULL;
  case SHOVELER_COMPONENT_LIGHT_TYPE_POINT: {
    int width =
        shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_WIDTH);
    int height =
        shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_HEIGHT);
    int samples =
        shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_SAMPLES);
    float ambientFactor = shovelerComponentGetFieldValueFloat(
        component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_AMBIENT_FACTOR);
    float exponentialFactor = shovelerComponentGetFieldValueFloat(
        component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_EXPONENTIAL_FACTOR);
    ShovelerVector3 color =
        shovelerComponentGetFieldValueVector3(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_COLOR);

    light = shovelerLightPointCreate(
        clientSystem->shaderCache,
        *position,
        width,
        height,
        samples,
        ambientFactor,
        exponentialFactor,
        color);
  } break;
  default:
    shovelerLogWarning("Trying to create light with unknown light type %d, ignoring.", type);
    return NULL;
  }

  shovelerSceneAddLight(clientSystem->scene, light);
  return light;
}

static void deactivateLightComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;
  ShovelerLight* light = (ShovelerLight*) component->systemData;

  shovelerShaderCacheInvalidateLight(clientSystem->shaderCache, light);
  shovelerSceneRemoveLight(clientSystem->scene, light);
}

static bool liveUpdateLightPositionDependency(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* clientSystemPointer) {
  ShovelerLight* light = (ShovelerLight*) component->systemData;

  ShovelerComponent* positionComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_LIGHT_FIELD_ID_POSITION);
  assert(positionComponent != NULL);
  const ShovelerVector3* position = shovelerComponentGetPosition(positionComponent);

  shovelerLightUpdatePosition(light, *position);

  return false; // don't propagate
}
