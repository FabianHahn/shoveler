#include "shoveler/component/image.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/resource.h"
#include "shoveler/component_system.h"
#include "shoveler/image.h"
#include "shoveler/image/png.h"
#include "shoveler/image/ppm.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateImageComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateImageComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddImageSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdImage);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateImageComponent;
  componentSystem->deactivateComponent = deactivateImageComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateImageComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* resourceComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE);
  assert(resourceComponent != NULL);

  const unsigned char* bufferData;
  int bufferSize;
  shovelerComponentGetResource(resourceComponent, &bufferData, &bufferSize);

  ShovelerComponentImageFormat format =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT);
  switch (format) {
  case SHOVELER_COMPONENT_IMAGE_FORMAT_PNG:
    return shovelerImagePngReadBuffer(bufferData, bufferSize);
  case SHOVELER_COMPONENT_IMAGE_FORMAT_PPM:
    return shovelerImagePpmReadBuffer(bufferData, bufferSize);
  default:
    shovelerLogWarning(
        "Failed to activate entity %lld component image: Unknown format value %d.",
        component->entityId,
        format);
    return NULL;
  }
}

static void deactivateImageComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerImage* image = component->systemData;

  shovelerImageFree(image);
}
