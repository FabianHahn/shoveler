#include "shoveler/component/sampler.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component_system.h"
#include "shoveler/sampler.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateSamplerComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateSamplerComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddSamplerSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdSampler);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateSamplerComponent;
  componentSystem->deactivateComponent = deactivateSamplerComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateSamplerComponent(ShovelerComponent* component, void* clientSystemPointer) {
  bool interpolate = shovelerComponentGetFieldValueBool(
      component, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_INTERPOLATE);
  bool useMipmaps = shovelerComponentGetFieldValueBool(
      component, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_USE_MIPMAPS);
  bool clamp =
      shovelerComponentGetFieldValueBool(component, SHOVELER_COMPONENT_SAMPLER_FIELD_ID_CLAMP);

  ShovelerSampler* sampler = shovelerSamplerCreate(interpolate, useMipmaps, clamp);
  return sampler;
}

static void deactivateSamplerComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerSampler* sampler = (ShovelerSampler*) component->systemData;

  shovelerSamplerFree(sampler);
}
