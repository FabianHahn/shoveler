#ifndef SHOVELER_SCHEMA_H
#define SHOVELER_SCHEMA_H

#include <glib.h>

typedef struct ShovelerComponentTypeStruct ShovelerComponentType;

typedef struct ShovelerSchemaStruct {
  /** map from string component type id to (ShovelerComponentType *) */
  GHashTable* componentTypes;
} ShovelerSchema;

ShovelerSchema* shovelerSchemaCreate();
// Adds a component type to the schema, transferring ownership to it.
bool shovelerSchemaAddComponentType(ShovelerSchema* schema, ShovelerComponentType* componentType);
ShovelerComponentType* shovelerSchemaGetComponentType(
    ShovelerSchema* schema, const char* componentTypeId);
void shovelerSchemaFree(ShovelerSchema* schema);

static inline bool shovelerSchemaHasComponentType(
    ShovelerSchema* schema, const char* componentTypeId) {
  return shovelerSchemaGetComponentType(schema, componentTypeId) != NULL;
}

#endif
