#include "shoveler/client_system.h"

#include <glib.h>
#include <stdlib.h>

#include "shoveler/colliders.h"
#include "shoveler/component/image.h"
#include "shoveler/component/position.h"
#include "shoveler/component/resource.h"
#include "shoveler/controller.h"
#include "shoveler/executor.h"
#include "shoveler/font.h"
#include "shoveler/game.h"
#include "shoveler/input.h"
#include "shoveler/log.h"
#include "shoveler/render_state.h"
#include "shoveler/scene.h"
#include "shoveler/schema.h"
#include "shoveler/schema/base.h"
#include "shoveler/schema/opengl.h"
#include "shoveler/shader_cache.h"
#include "shoveler/system.h"
#include "shoveler/world.h"
#include "shoveler/world_dependency_graph.h"

static void clientSystemUpdateAuthoritativeComponent(
    ShovelerWorld* world,
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* clientSystemPointer);
static void clientSystemKeyHandler(
    ShovelerInput* input, int key, int scancode, int action, int mods, void* clientSystemPointer);
static void clientSystemUpdateWorldCounters(void* clientSystemPointer);

ShovelerClientSystem* shovelerClientSystemCreate(
    ShovelerGame* game,
    ShovelerClientSystemUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent,
    void* updateAuthoritativeComponentUserData) {
  ShovelerClientSystem* clientSystem = malloc(sizeof(ShovelerClientSystem));
  clientSystem->system = shovelerSystemCreate();
  clientSystem->schema = shovelerSchemaCreate();
  clientSystem->world = shovelerWorldCreate(
      clientSystem->schema,
      clientSystem->system,
      clientSystemUpdateAuthoritativeComponent,
      clientSystem);
  clientSystem->executor = game->updateExecutor;
  clientSystem->input = game->input;
  clientSystem->colliders = game->colliders;
  clientSystem->fonts = game->fonts;
  clientSystem->controller = game->controller;
  clientSystem->shaderCache = game->shaderCache;
  clientSystem->scene = game->scene;
  clientSystem->renderState = &game->renderState;
  clientSystem->updateAuthoritativeComponent = updateAuthoritativeComponent;
  clientSystem->updateAuthoritativeComponentUserData = updateAuthoritativeComponentUserData;
  clientSystem->keyCallback =
      shovelerInputAddKeyCallback(game->input, clientSystemKeyHandler, clientSystem);
  clientSystem->updateWorldCountersExecutorCallback = shovelerExecutorSchedulePeriodic(
      clientSystem->executor,
      /* timeoutMs */ 0,
      /* intervalMs */ 1000,
      clientSystemUpdateWorldCounters,
      clientSystem);
  clientSystem->lastWorldCounters.numEntities = 0;
  clientSystem->lastWorldCounters.numComponents = 0;
  clientSystem->lastWorldCounters.numComponentDependencies = 0;

  shovelerSchemaBaseRegister(clientSystem->schema);
  shovelerSchemaOpenglRegister(clientSystem->schema);

  shovelerClientSystemAddImageSystem(clientSystem);
  shovelerClientSystemAddPositionSystem(clientSystem);

  return clientSystem;
}

void shovelerClientSystemFree(ShovelerClientSystem* clientSystem) {
  shovelerExecutorRemoveCallback(
      clientSystem->executor, clientSystem->updateWorldCountersExecutorCallback);
  shovelerInputRemoveKeyCallback(clientSystem->input, clientSystem->keyCallback);
  shovelerWorldFree(clientSystem->world);
  shovelerSchemaFree(clientSystem->schema);
  shovelerSystemFree(clientSystem->system);
  free(clientSystem);
}

static void clientSystemUpdateAuthoritativeComponent(
    ShovelerWorld* world,
    ShovelerComponent* component,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  if (clientSystem->updateAuthoritativeComponent != NULL) {
    clientSystem->updateAuthoritativeComponent(
        clientSystem, component, field, value, clientSystem->updateAuthoritativeComponentUserData);
  }
}

static void clientSystemKeyHandler(
    ShovelerInput* input, int key, int scancode, int action, int mods, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
    shovelerLogInfo("F9 key pressed, writing world dependency graph.");
    shovelerWorldDependencyGraphWrite(clientSystem->world, "world_dependencies.dot");
  }
}

static void clientSystemUpdateWorldCounters(void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  bool worldCountersChanged = clientSystem->lastWorldCounters.numEntities !=
          g_hash_table_size(clientSystem->world->entities) ||
      clientSystem->lastWorldCounters.numComponents != clientSystem->world->numComponents ||
      clientSystem->lastWorldCounters.numComponentDependencies !=
          clientSystem->world->numComponentDependencies;

  if (worldCountersChanged) {
    clientSystem->lastWorldCounters.numEntities = g_hash_table_size(clientSystem->world->entities);
    clientSystem->lastWorldCounters.numComponents = clientSystem->world->numComponents;
    clientSystem->lastWorldCounters.numComponentDependencies =
        clientSystem->world->numComponentDependencies;

    shovelerLogInfo(
        "World changed: %u entities, %u components, %u dependencies.",
        clientSystem->lastWorldCounters.numEntities,
        clientSystem->lastWorldCounters.numComponents,
        clientSystem->lastWorldCounters.numComponentDependencies);
  }
}
