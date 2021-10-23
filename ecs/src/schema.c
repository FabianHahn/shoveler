#include "shoveler/schema.h"

#include <assert.h>
#include <stdlib.h> // malloc, free

#include "shoveler/component_type.h"

static void freeComponentType(void* componentTypePointer);

ShovelerSchema* shovelerSchemaCreate() {
  ShovelerSchema* schema = malloc(sizeof(ShovelerSchema));
  schema->componentTypes = g_hash_table_new_full(
      g_str_hash, g_str_equal, /* key_destroy_func */ NULL, freeComponentType);
  return schema;
}

bool shovelerSchemaAddComponentType(ShovelerSchema* schema, ShovelerComponentType* componentType) {
  return g_hash_table_insert(schema->componentTypes, (gpointer) componentType->id, componentType);
}

ShovelerComponentType *shovelerSchemaGetComponentType(
    ShovelerSchema* schema, const char *componentTypeId) {
  return g_hash_table_lookup(schema->componentTypes, componentTypeId);
}

void shovelerSchemaFree(ShovelerSchema* schema) {
  g_hash_table_destroy(schema->componentTypes);
  free(schema);
}

static void freeComponentType(void* componentTypePointer) {
  ShovelerComponentType* componentType = componentTypePointer;
  shovelerComponentTypeFree(componentType);
}
