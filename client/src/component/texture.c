#include "shoveler/component/texture.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/image.h"
#include "shoveler/component/text_texture_renderer.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"
#include "shoveler/text_texture_renderer.h"
#include "shoveler/texture.h"

static void* activateTextureComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTextureComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTextureSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTexture);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTextureComponent;
  componentSystem->deactivateComponent = deactivateTextureComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTextureComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerTexture* texture;

  ShovelerComponentTextureType type = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TYPE);
  switch (type) {
  case SHOVELER_COMPONENT_TEXTURE_TYPE_IMAGE: {
    if (!shovelerComponentHasFieldValue(
            component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE)) {
      shovelerLogWarning(
          "Failed to activate texture component of entity %lld: No image dependency specified",
          component->entityId);
      return NULL;
    }

    ShovelerComponent* imageComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_IMAGE);
    assert(imageComponent != NULL);
    ShovelerImage* image = shovelerComponentGetImage(imageComponent);
    assert(image != NULL);

    texture = shovelerTextureCreate2d(image, false);
    shovelerTextureUpdate(texture);
  } break;
  case SHOVELER_COMPONENT_TEXTURE_TYPE_TEXT: {
    if (!shovelerComponentHasFieldValue(
            component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT_TEXTURE_RENDERER)) {
      shovelerLogWarning(
          "Failed to activate texture component of entity %lld: No text texture renderer "
          "dependency specified",
          component->entityId);
      return NULL;
    }

    ShovelerComponent* textTextureRendererComponent = shovelerComponentGetDependency(
        component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT_TEXTURE_RENDERER);
    assert(textTextureRendererComponent != NULL);
    ShovelerTextTextureRenderer* textTextureRenderer =
        shovelerComponentGetTextTextureRenderer(textTextureRendererComponent);
    assert(textTextureRenderer != NULL);

    if (!shovelerComponentHasFieldValue(
            component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT)) {
      shovelerLogWarning(
          "Failed to activate texture component of entity %lld: No text to render specified",
          component->entityId);
      return NULL;
    }

    const char* text = shovelerComponentGetFieldValueString(
        component, SHOVELER_COMPONENT_TEXTURE_FIELD_ID_TEXT);
    assert(text != NULL);

    texture = shovelerTextTextureRendererRender(textTextureRenderer, text, clientSystem->renderState);
  } break;
  default:
    shovelerLogWarning(
        "Failed to activate texture component of entity %lld: Unknown texture type %d",
        component->entityId,
        type);
    return NULL;
  }

  return texture;
}

static void deactivateTextureComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerTexture* texture = (ShovelerTexture*) component->systemData;

  shovelerTextureFree(texture);
}
