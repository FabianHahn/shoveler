#include "shoveler/component/font.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/resource.h"
#include "shoveler/component_system.h"
#include "shoveler/font.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateFontComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateFontComponent(ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddFontSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdFont);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateFontComponent;
  componentSystem->deactivateComponent = deactivateFontComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateFontComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerComponent* resourceComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_FONT_FIELD_ID_RESOURCE);
  assert(resourceComponent != NULL);

  const char* name =
      shovelerComponentGetFieldValueString(component, SHOVELER_COMPONENT_FONT_FIELD_ID_NAME);

  const unsigned char* bufferData;
  int bufferSize;
  shovelerComponentGetResource(resourceComponent, &bufferData, &bufferSize);

  ShovelerFont* font =
      shovelerFontsLoadFontBuffer(clientSystem->fonts, name, bufferData, bufferSize);
  return font;
}

static void deactivateFontComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerFont* font = component->systemData;
  shovelerFontsUnloadFont(clientSystem->fonts, font->name);
}
