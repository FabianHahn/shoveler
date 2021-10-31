#include "shoveler/component/text_sprite.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/material.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/sprite/text.h"
#include "shoveler/system.h"

static void* activateTextSpriteComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTextSpriteComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTextSpriteSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTextSprite);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTextSpriteComponent;
  componentSystem->deactivateComponent = deactivateTextSpriteComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTextSpriteComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* materialComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXT_SPRITE_FIELD_ID_MATERIAL);
  assert(materialComponent != NULL);
  ShovelerMaterial* material = shovelerComponentGetMaterial(materialComponent);
  assert(material != NULL);

  // TODO: get TextSprite atlast texture

  // TODO: create text sprite
  shovelerLogWarning(
      "Failed to activate text component on entity %lld - not implemented yet.",
      component->entityId);
  return NULL;
}

static void deactivateTextSpriteComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerSprite* textSprite = (ShovelerSprite*) component->systemData;
  assert(textSprite != NULL);

  shovelerSpriteFree(textSprite);
}
