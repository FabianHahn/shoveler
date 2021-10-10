#ifndef SHOVELER_ENTITY_COMPONENT_ID_H
#define SHOVELER_ENTITY_COMPONENT_ID_H

#include <glib.h>
#include <shoveler/hash.h>
#include <stdlib.h>

typedef struct ShovelerEntityComponentIdStruct {
  long long int entityId;
  const char* componentTypeId;
} ShovelerEntityComponentId;

static inline ShovelerEntityComponentId shovelerEntityComponentId(
    long long int entityId, const char* componentTypeId) {
  ShovelerEntityComponentId entityComponentId;
  entityComponentId.entityId = entityId;
  entityComponentId.componentTypeId = componentTypeId;
  return entityComponentId;
}

static inline ShovelerEntityComponentId* shovelerEntityComponentIdCreate(
    long long int entityId, const char* componentTypeId) {
  ShovelerEntityComponentId* entityComponentId =
      (ShovelerEntityComponentId*) malloc(sizeof(ShovelerEntityComponentId));
  entityComponentId->entityId = entityId;
  entityComponentId->componentTypeId = componentTypeId;
  return entityComponentId;
}

static inline ShovelerEntityComponentId* shovelerEntityComponentIdCopy(
    const ShovelerEntityComponentId* other) {
  return shovelerEntityComponentIdCreate(other->entityId, other->componentTypeId);
}

static inline guint shovelerEntityComponentIdHash(gconstpointer entityComponentIdPointer) {
  const ShovelerEntityComponentId* entityComponentId =
      (const ShovelerEntityComponentId*) entityComponentIdPointer;

  guint entityIdHash = g_int64_hash(&entityComponentId->entityId);
  guint componentTypeIdHash = g_str_hash(entityComponentId->componentTypeId);

  return shovelerHashCombine(entityIdHash, componentTypeIdHash);
}

static inline gboolean shovelerEntityComponentIdEqual(
    gconstpointer aPointer, gconstpointer bPointer) {
  const ShovelerEntityComponentId* a = (const ShovelerEntityComponentId*) aPointer;
  const ShovelerEntityComponentId* b = (const ShovelerEntityComponentId*) bPointer;

  return a->entityId == b->entityId && a->componentTypeId == b->componentTypeId;
}

static inline void shovelerEntityComponentIdAssign(
    ShovelerEntityComponentId* entityComponentId, const ShovelerEntityComponentId* other) {
  entityComponentId->entityId = other->entityId;
  entityComponentId->componentTypeId = other->componentTypeId;
}

#endif
