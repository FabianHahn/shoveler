#include "shoveler/component/text_texture_renderer.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/font_atlas_texture.h"
#include "shoveler/component_system.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"
#include "shoveler/text_texture_renderer.h"

static void* activateTextTextureRendererComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTextTextureRendererComponent(
    ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTextTextureRendererSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType = shovelerSchemaGetComponentType(
      clientSystem->schema, shovelerComponentTypeIdTextTextureRenderer);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTextTextureRendererComponent;
  componentSystem->deactivateComponent = deactivateTextTextureRendererComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTextTextureRendererComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerComponent* fontAtlasTextureComponent = shovelerComponentGetDependency(
      component, SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_FIELD_ID_FONT_ATLAS_TEXTURE);
  assert(fontAtlasTextureComponent != NULL);
  ShovelerFontAtlasTexture* fontAtlasTexture =
      shovelerComponentGetFontAtlasTexture(fontAtlasTextureComponent);
  assert(fontAtlasTexture != NULL);

  return shovelerTextTextureRendererCreate(fontAtlasTexture, clientSystem->shaderCache);
}

static void deactivateTextTextureRendererComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerTextTextureRenderer* textTextureRenderer = component->systemData;

  shovelerTextTextureRendererFree(textTextureRenderer);
}
