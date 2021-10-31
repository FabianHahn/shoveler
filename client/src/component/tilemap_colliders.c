#include "shoveler/component/tilemap_colliders.h"

#include <assert.h>
#include <stdlib.h>

#include "shoveler/client_system.h"
#include "shoveler/component_system.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateTilemapCollidersComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTilemapCollidersComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdateCollidersOption(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* clientSystemPointer);
static void updateColliders(ShovelerComponent* component, bool* colliders);

void shovelerClientSystemAddTilemapCollidersSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTilemapColliders);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTilemapCollidersComponent;
  componentSystem->deactivateComponent = deactivateTilemapCollidersComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS]
      .liveUpdateField = liveUpdateCollidersOption;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTilemapCollidersComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  int numColumns = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
  int numRows = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);

  bool* colliders = malloc(numColumns * numRows * sizeof(bool));
  updateColliders(component, colliders);

  return colliders;
}

static void deactivateTilemapCollidersComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  bool* colliders = (bool*) component->systemData;

  free(colliders);
}

static bool liveUpdateCollidersOption(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* clientSystemPointer) {
  bool* colliders = (bool*) component->systemData;
  assert(colliders != NULL);

  updateColliders(component, colliders);

  return false; // don't propagate
}

static void updateColliders(ShovelerComponent* component, bool* colliders) {
  int numColumns = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
  int numRows = shovelerComponentGetFieldValueInt(
      component, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);

  const unsigned char* collidersOption;
  shovelerComponentGetFieldValueBytes(
      component,
      SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_COLLIDERS,
      &collidersOption,
      /* outputSize */ NULL);
  assert(collidersOption != NULL);

  for (int row = 0; row < numRows; ++row) {
    int rowIndex = row * numColumns;
    for (int column = 0; column < numColumns; ++column) {
      int columnIndex = rowIndex + column;

      colliders[columnIndex] = collidersOption[columnIndex];
    }
  }
}
