#include "shoveler/system.h"

#include <assert.h>
#include <stdlib.h> // malloc, free

#include "shoveler/component_system.h"
#include "shoveler/component_type.h"

static void freeComponentSystem(void* shovelerComponentSystemPointer);

ShovelerSystem* shovelerSystemCreate() {
  ShovelerSystem* system = malloc(sizeof(ShovelerSystem));
  system->componentSystems = g_hash_table_new_full(
      g_str_hash, g_str_equal, /* key_destroy_func */ NULL, freeComponentSystem);

  return system;
}

ShovelerComponentSystem* shovelerSystemForComponentType(
    ShovelerSystem* system, ShovelerComponentType* componentType) {
  ShovelerComponentSystem* componentSystem =
      g_hash_table_lookup(system->componentSystems, componentType->id);
  if (componentSystem == NULL) {
    componentSystem = shovelerComponentSystemCreate(componentType);
    bool inserted =
        g_hash_table_insert(system->componentSystems, (gpointer) componentType->id, componentSystem);
    assert(inserted);
  }

  return componentSystem;
}

void shovelerSystemFree(ShovelerSystem* system) {
  g_hash_table_destroy(system->componentSystems);
  free(system);
}

static void freeComponentSystem(void* shovelerComponentSystemPointer) {
  ShovelerComponentSystem* componentSystem = shovelerComponentSystemPointer;
  shovelerComponentSystemFree(componentSystem);
}
