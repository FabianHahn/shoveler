#include "shoveler/component/font_atlas.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/font.h"
#include "shoveler/component_system.h"
#include "shoveler/font_atlas.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateFontAtlasComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateFontAtlasComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddFontAtlasSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdFontAtlas);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateFontAtlasComponent;
  componentSystem->deactivateComponent = deactivateFontAtlasComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateFontAtlasComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* fontComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT);
  assert(fontComponent != NULL);
  ShovelerFont* font = shovelerComponentGetFont(fontComponent);
  assert(font != NULL);

  int fontSize = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT_SIZE);
  int padding = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_PADDING);

  return shovelerFontAtlasCreate(font, fontSize, padding);
}

static void deactivateFontAtlasComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerFontAtlas* fontAtlas = component->systemData;

  shovelerFontAtlasFree(fontAtlas);
}
