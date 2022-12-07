#include <shoveler/camera/perspective.h>
#include <shoveler/client_system.h>
#include <shoveler/component.h>
#include <shoveler/component_field.h>
#include <shoveler/component_type.h>
#include <shoveler/constants.h>
#include <shoveler/file.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image/png.h>
#include <shoveler/map.h>
#include <shoveler/material/texture.h>
#include <shoveler/opengl.h>
#include <shoveler/types.h>
#include <shoveler/world.h>
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include "seeder.h"

static double time = 0.0;

static void updateGame(ShovelerGame* game, double dt);
static void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* userData);

static ShovelerWorld* world = NULL;

int main(int argc, char* argv[]) {
  if (argc != 9) {
    shovelerLogError(
        "Usage:\n\t%s <tileset png> <tileset columns> <tileset rows> <character png> <character2 "
        "png> <character3 png> <character4 png> <character shift amount>",
        argv[0]);
    return EXIT_FAILURE;
  }

  const char* tilesetPngFilename = argv[1];
  int tilesetPngColumns = atoi(argv[2]);
  int tilesetPngRows = atoi(argv[3]);
  const char* character1PngFilename = argv[4];
  const char* character2PngFilename = argv[5];
  const char* character3PngFilename = argv[6];
  const char* character4PngFilename = argv[7];
  int characterShiftAmount = atoi(argv[8]);

  ShovelerGameWindowSettings windowSettings;
  windowSettings.windowTitle = "shoveler";
  windowSettings.fullscreen = false;
  windowSettings.vsync = true;
  windowSettings.samples = 4;
  windowSettings.windowedWidth = 640;
  windowSettings.windowedHeight = 480;

  ShovelerGameCameraSettings cameraSettings;
  cameraSettings.frame.position = shovelerVector3(0, 0, 5);
  cameraSettings.frame.direction = shovelerVector3(0, 0, -1);
  cameraSettings.frame.up = shovelerVector3(0, 1, 0);
  cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
  cameraSettings.projection.aspectRatio =
      (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
  cameraSettings.projection.nearClippingPlane = 0.01;
  cameraSettings.projection.farClippingPlane = 1000;

  ShovelerGameControllerSettings controllerSettings;
  controllerSettings.frame = cameraSettings.frame;
  controllerSettings.moveFactor = 2.0f;
  controllerSettings.tiltFactor = 0.0005f;
  controllerSettings.boundingBoxSize2 = 0.9f;
  controllerSettings.boundingBoxSize3 = 0.0f;

  shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
  shovelerGlobalInit();

  ShovelerGame* game =
      shovelerGameCreate(updateGame, &windowSettings, &cameraSettings, &controllerSettings);
  if (game == NULL) {
    return EXIT_FAILURE;
  }

  game->controller->lockTiltX = true;
  game->controller->lockTiltY = true;

  int chunkSize = 10;
  int numChunkRows = 10;
  int numChunkColumns = 10;
  ShovelerMap* map = shovelerMapGenerate(chunkSize, numChunkRows, numChunkColumns);

  ShovelerWorldCallbacks worldCallbacks = shovelerWorldCallbacks();
  worldCallbacks.onUpdateComponent = onUpdateComponent;
  ShovelerClientSystem* clientSystem = shovelerClientSystemCreate(game, &worldCallbacks);
  world = clientSystem->world;

  long long int nextEntityId = 1;
  shovelerClientTilesSeederInit(
      world,
      map,
      &nextEntityId,
      tilesetPngFilename,
      tilesetPngColumns,
      tilesetPngRows,
      character1PngFilename,
      character2PngFilename,
      character3PngFilename,
      character4PngFilename,
      characterShiftAmount);

  shovelerOpenGLCheckSuccess();

  while (shovelerGameIsRunning(game)) {
    shovelerGameRenderFrame(game);
  }
  shovelerLogInfo("Exiting main loop, goodbye.");

  shovelerClientSystemFree(clientSystem);
  shovelerMapFree(map);
  shovelerGameFree(game);
  shovelerGlobalUninit();
  shovelerLogTerminate();

  return EXIT_SUCCESS;
}

static void updateGame(ShovelerGame* game, double dt) {
  shovelerCameraUpdateView(game->camera);

  time += dt;
}

static void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* userData) {
  if (!isAuthoritative) {
    return;
  }

  GString* printedValue = shovelerComponentFieldPrintValue(value);

  shovelerLogTrace(
      "Updating field '%s' of component '%s' on entity %lld: %s",
      field->name,
      component->type->id,
      component->entityId,
      printedValue->str);

  g_string_free(printedValue, true);
}
