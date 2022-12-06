#ifndef SHOVELER_COMPONENT_TYPE_INDEXER_H
#define SHOVELER_COMPONENT_TYPE_INDEXER_H

#include <glib.h>
#include <stdbool.h>

typedef struct ShovelerComponentTypeIndexerStruct {
  GArray* componentTypes;
  GHashTable* componentTypeIndexLookup;
} ShovelerComponentTypeIndexer;

ShovelerComponentTypeIndexer* shovelerComponentTypeIndexerCreate();
void shovelerComponentTypeIndexerFree(ShovelerComponentTypeIndexer* componentTypeIndexer);
bool shovelerComponentTypeIndexerAddComponentType(
    ShovelerComponentTypeIndexer* componentTypeIndexer, const char* componentTypeId);
int shovelerComponentTypeIndexerFromId(
    ShovelerComponentTypeIndexer* componentTypeIndexer, const char* componentTypeId);
const char* shovelerComponentTypeIndexerToId(
    ShovelerComponentTypeIndexer* componentTypeIndexer, int componentTypeIndex);

#endif