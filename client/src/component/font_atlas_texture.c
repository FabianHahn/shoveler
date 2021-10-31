#include "shoveler/component/font_atlas_texture.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/font_atlas.h"
#include "shoveler/component_system.h"
#include "shoveler/font_atlas_texture.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateFontAtlasTextureComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static void deactivateFontAtlasTextureComponent(
    ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddFontAtlasTextureSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdFontAtlasTexture);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateFontAtlasTextureComponent;
  componentSystem->deactivateComponent = deactivateFontAtlasTextureComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateFontAtlasTextureComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* fontAtlasComponent = shovelerComponentGetDependency(
      component, SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_FIELD_ID_FONT_ATLAS);
  assert(fontAtlasComponent != NULL);
  ShovelerFontAtlas* fontAtlas = shovelerComponentGetFontAtlas(fontAtlasComponent);
  assert(fontAtlas != NULL);

  ShovelerFontAtlasTexture* fontAtlasTexture = shovelerFontAtlasTextureCreate(fontAtlas);
  shovelerFontAtlasTextureUpdate(fontAtlasTexture);

  return fontAtlasTexture;
}

static void deactivateFontAtlasTextureComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerFontAtlasTexture* fontAtlasTexture = component->systemData;

  shovelerFontAtlasTextureFree(fontAtlasTexture);
}
