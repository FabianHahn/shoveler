#include "shoveler/component_type_indexer.h"

#include <assert.h>
#include <shoveler/log.h>
#include <stdlib.h>

ShovelerComponentTypeIndexer* shovelerComponentTypeIndexerCreate() {
  ShovelerComponentTypeIndexer* componentTypeIndexer = malloc(sizeof(ShovelerComponentTypeIndexer));
  componentTypeIndexer->componentTypes =
      g_array_new(/* zeroTerminated */ false, /* clear */ false, sizeof(const char*));
  componentTypeIndexer->componentTypeIndexLookup = g_hash_table_new(g_direct_hash, g_direct_equal);
  return componentTypeIndexer;
}

void shovelerComponentTypeIndexerFree(ShovelerComponentTypeIndexer* componentTypeIndexer) {
  g_hash_table_destroy(componentTypeIndexer->componentTypeIndexLookup);
  g_array_free(componentTypeIndexer->componentTypes, /* freeSegment */ true);
  free(componentTypeIndexer);
}

bool shovelerComponentTypeIndexerAddComponentType(
    ShovelerComponentTypeIndexer* componentTypeIndexer, const char* componentTypeId) {
  if (g_hash_table_contains(componentTypeIndexer->componentTypeIndexLookup, componentTypeId)) {
    return false;
  }

  int index = (int) componentTypeIndexer->componentTypes->len;
  g_array_append_val(componentTypeIndexer->componentTypes, componentTypeId);

  uint64_t index64 = index;
  g_hash_table_insert(
      componentTypeIndexer->componentTypeIndexLookup,
      (gpointer) componentTypeId,
      (gpointer) index64);
  return true;
}

int shovelerComponentTypeIndexerFromId(
    ShovelerComponentTypeIndexer* componentTypeIndexer, const char* componentTypeId) {
  int64_t value;
  if (!g_hash_table_lookup_extended(
          componentTypeIndexer->componentTypeIndexLookup,
          componentTypeId,
          /* origKey */ NULL,
          (gpointer*) &value)) {
    return -1;
  }

  int index = (int) value;
  assert(index >= 0);
  assert(index < componentTypeIndexer->componentTypes->len);
  return index;
}

const char* shovelerComponentTypeIndexerToId(
    ShovelerComponentTypeIndexer* componentTypeIndexer, int componentTypeIndex) {
  if (componentTypeIndex < 0 || componentTypeIndex >= componentTypeIndexer->componentTypes->len) {
    return NULL;
  }

  return g_array_index(componentTypeIndexer->componentTypes, const char*, componentTypeIndex);
}
