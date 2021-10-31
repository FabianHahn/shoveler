#include "shoveler/component/tilemap_tiles.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/image.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/image.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"
#include "shoveler/texture.h"

static void* activateTilemapTilesComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTilemapTilesComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static bool liveUpdateTilesField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData);
static void updateTiles(ShovelerComponent* component, ShovelerTexture* texture);
static bool isComponentImageResourceEntityDefinition(ShovelerComponent* component);
static bool isComponentConfigurationOptionDefinition(ShovelerComponent* component);

void shovelerClientSystemAddTilemapTilesSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTilemapTiles);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTilemapTilesComponent;
  componentSystem->deactivateComponent = deactivateTilemapTilesComponent;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS]
      .liveUpdateField = liveUpdateTilesField;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS]
      .liveUpdateField = liveUpdateTilesField;
  componentSystem->fieldOptions[SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS]
      .liveUpdateField = liveUpdateTilesField;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTilemapTilesComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  bool isImageResourceEntityDefinition = isComponentImageResourceEntityDefinition(component);
  bool isConfigurationOptionDefinition = isComponentConfigurationOptionDefinition(component);

  ShovelerTexture* texture;
  if (isImageResourceEntityDefinition && !isConfigurationOptionDefinition) {
    ShovelerComponent* imageComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE);
    assert(imageComponent != NULL);
    ShovelerImage* image = shovelerComponentGetImage(imageComponent);
    assert(image != NULL);

    texture = shovelerTextureCreate2d(image, false);
    shovelerTextureUpdate(texture);
  } else if (!isImageResourceEntityDefinition && isConfigurationOptionDefinition) {
    int numColumns = shovelerComponentGetFieldValueInt(
        component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS);
    int numRows = shovelerComponentGetFieldValueInt(
        component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS);

    ShovelerImage* tilemapImage = shovelerImageCreate(numColumns, numRows, /* channels */ 3);
    texture = shovelerTextureCreate2d(tilemapImage, true);
    updateTiles(component, texture);
  } else {
    shovelerLogWarning(
        "Failed to activate tilemap tiles of entity %lld because it doesn't provide either an "
        "image resource entity definition or a configuration option definition",
        component->entityId);
    return NULL;
  }

  return texture;
}

static void deactivateTilemapTilesComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerTexture* texture = (ShovelerTexture*) component->systemData;

  shovelerTextureFree(texture);
}

static bool liveUpdateTilesField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData) {
  ShovelerTexture* texture = (ShovelerTexture*) component->systemData;
  assert(texture != NULL);

  if (!isComponentImageResourceEntityDefinition(component) &&
      isComponentConfigurationOptionDefinition(component)) {
    updateTiles(component, texture);
  }

  return false; // don't propagate
}

static void updateTiles(ShovelerComponent* component, ShovelerTexture* texture) {
  const unsigned char* tilesetColumns;
  const unsigned char* tilesetRows;
  const unsigned char* tilesetIds;
  shovelerComponentGetFieldValueBytes(
      component,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS,
      &tilesetColumns,
      /* outputSize */ NULL);
  shovelerComponentGetFieldValueBytes(
      component,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS,
      &tilesetRows,
      /* outputSize */ NULL);
  shovelerComponentGetFieldValueBytes(
      component,
      SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS,
      &tilesetIds,
      /* outputSize */ NULL);
  assert(tilesetColumns != NULL);
  assert(tilesetRows != NULL);
  assert(tilesetIds != NULL);

  ShovelerImage* tilemapImage = texture->image;
  int numColumns = tilemapImage->width;
  int numRows = tilemapImage->height;

  for (int row = 0; row < numRows; ++row) {
    int rowIndex = row * numColumns;
    for (int column = 0; column < numColumns; ++column) {
      int columnIndex = rowIndex + column;

      shovelerImageGet(tilemapImage, column, row, 0) = tilesetColumns[columnIndex];
      shovelerImageGet(tilemapImage, column, row, 1) = tilesetRows[columnIndex];
      shovelerImageGet(tilemapImage, column, row, 2) = tilesetIds[columnIndex];
    }
  }

  shovelerTextureUpdate(texture);
}

static bool isComponentImageResourceEntityDefinition(ShovelerComponent* component) {
  return shovelerComponentHasFieldValue(
      component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_IMAGE);
}

static bool isComponentConfigurationOptionDefinition(ShovelerComponent* component) {
  return shovelerComponentHasFieldValue(
             component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_COLUMNS) &&
      shovelerComponentHasFieldValue(
             component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_NUM_ROWS) &&
      shovelerComponentHasFieldValue(
             component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_COLUMNS) &&
      shovelerComponentHasFieldValue(
             component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_ROWS) &&
      shovelerComponentHasFieldValue(
             component, SHOVELER_COMPONENT_TILEMAP_TILES_OPTION_TILESET_IDS);
}
