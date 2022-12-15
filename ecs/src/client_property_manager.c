#include "shoveler/client_property_manager.h"

#include <assert.h>
#include <shoveler/log.h>
#include <stdlib.h>

static_assert(sizeof(gpointer) == sizeof(int64_t), "pointers must be 64 bits");
static_assert(sizeof(gpointer) == sizeof(long long int), "pointers must be 64 bits");

#define CLIENT_ID_TO_POINTER(i) ((gpointer)(long long int) (i))
#define ENTITY_ID_TO_POINTER(i) ((gpointer)(long long int) (i))
#define POINTER_TO_CLIENT_ID(i) ((int64_t)(long long int) (i))
#define POINTER_TO_ENTITY_ID(i) ((long long int) (i))

static ShovelerClientPropertyManagerClientEntityProperties* createOrGetClientEntityProperties(
    ShovelerClientPropertyManagerClientProperties* clientProperties, long long int entityId);
static ShovelerClientPropertyManagerEntityProperties* createOrGetEntityProperties(
    ShovelerClientPropertyManager* clientPropertyManager, long long int entityId);
static void freeClientEntityProperties(void* clientEntityPropertiesPointer);
static void freeClientProperties(void* clientPropertiesPointer);
static void freeEntityProperties(void* entityPropertiesPointer);

ShovelerClientPropertyManager* shovelerClientPropertyManagerCreate() {
  ShovelerClientPropertyManager* clientPropertyManager =
      malloc(sizeof(ShovelerClientPropertyManager));
  clientPropertyManager->clientProperties = g_hash_table_new_full(
      g_int64_hash, g_int64_equal, /* keyDestroyFunc */ NULL, freeClientProperties);
  clientPropertyManager->entityProperties = g_hash_table_new_full(
      g_int64_hash, g_int64_equal, /* keyDestroyFunc */ NULL, freeEntityProperties);
  return clientPropertyManager;
}

void shovelerClientPropertyManagerFree(ShovelerClientPropertyManager* clientPropertyManager) {
  g_hash_table_destroy(clientPropertyManager->entityProperties);
  g_hash_table_destroy(clientPropertyManager->clientProperties);
  free(clientPropertyManager);
}

bool shovelerClientPropertyManagerAddClient(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId) {
  if (g_hash_table_contains(clientPropertyManager->clientProperties, &clientId)) {
    return false;
  }

  ShovelerClientPropertyManagerClientProperties* clientProperties =
      malloc(sizeof(ShovelerClientPropertyManagerClientProperties));
  clientProperties->clientId = clientId;
  clientProperties->interestedEntities = g_hash_table_new(g_direct_hash, g_direct_equal);
  clientProperties->entityProperties = g_hash_table_new_full(
      g_int64_hash, g_int64_equal, /* keyDestroyFunc */ NULL, freeClientEntityProperties);
  g_hash_table_insert(
      clientPropertyManager->clientProperties, &clientProperties->clientId, clientProperties);
  return true;
}

bool shovelerClientPropertyManagerRemoveClient(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId) {
  return g_hash_table_remove(clientPropertyManager->clientProperties, &clientId);
}

bool shovelerClientPropertyManagerAddEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  if (g_hash_table_contains(clientProperties->interestedEntities, &entityId)) {
    return false;
  }

  g_hash_table_add(clientProperties->interestedEntities, ENTITY_ID_TO_POINTER(entityId));

  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      createOrGetEntityProperties(clientPropertyManager, entityId);
  g_hash_table_add(entityProperties->interestedClients, CLIENT_ID_TO_POINTER(clientId));

  return true;
}

bool shovelerClientPropertyManagerRemoveEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  if (!g_hash_table_contains(
          clientProperties->interestedEntities, ENTITY_ID_TO_POINTER(entityId))) {
    return false;
  }

  g_hash_table_remove(clientProperties->interestedEntities, ENTITY_ID_TO_POINTER(entityId));

  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      g_hash_table_lookup(clientPropertyManager->entityProperties, &entityId);
  assert(entityProperties != NULL);
  g_hash_table_remove(entityProperties->interestedClients, CLIENT_ID_TO_POINTER(clientId));

  return true;
}

bool shovelerClientPropertyManagerHasEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  return g_hash_table_contains(
      clientProperties->interestedEntities, ENTITY_ID_TO_POINTER(entityId));
}

void shovelerClientPropertyManagerGetEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager,
    long long int entityId,
    const char* componentTypeId,
    GArray* outputArray) {
  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      g_hash_table_lookup(clientPropertyManager->entityProperties, &entityId);
  if (entityProperties == NULL) {
    g_array_set_size(outputArray, 0);
    return;
  }

  bool filterAuthoritativeComponent = componentTypeId != NULL;
  int64_t authoritativeClientId = 0;
  if (componentTypeId != NULL) {
    gpointer value;
    if (g_hash_table_lookup_extended(
            entityProperties->componentAuthority,
            componentTypeId,
            /* origKey */ NULL,
            &value)) {
      authoritativeClientId = POINTER_TO_CLIENT_ID(value);
    } else {
      // If we cannot find the component, that is equivalent to not filtering by authoritative
      // component.
      filterAuthoritativeComponent = false;
    }
  }

  g_array_set_size(outputArray, 0);

  GHashTableIter iter;
  gpointer clientIdValue;
  g_hash_table_iter_init(&iter, entityProperties->interestedClients);
  int i = 0;
  while (g_hash_table_iter_next(&iter, (gpointer*) &clientIdValue, /* value */ NULL)) {
    int64_t clientId = POINTER_TO_CLIENT_ID(clientIdValue);

    if (filterAuthoritativeComponent && clientId == authoritativeClientId) {
      continue;
    }

    g_array_append_val(outputArray, clientId);
    i++;
  }
}

bool shovelerClientPropertyManagerAddComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      createOrGetEntityProperties(clientPropertyManager, entityId);
  if (g_hash_table_contains(entityProperties->componentAuthority, componentTypeId)) {
    return false;
  }

  g_hash_table_insert(
      entityProperties->componentAuthority,
      (gpointer) componentTypeId,
      CLIENT_ID_TO_POINTER(clientId));

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      createOrGetClientEntityProperties(clientProperties, entityId);
  assert(!g_hash_table_contains(clientEntityProperties->authoritativeComponents, componentTypeId));
  g_hash_table_add(clientEntityProperties->authoritativeComponents, (gpointer) componentTypeId);

  return true;
}

bool shovelerClientPropertyManagerRemoveComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      g_hash_table_lookup(clientPropertyManager->entityProperties, &entityId);
  if (entityProperties == NULL) {
    return false;
  }

  gpointer value;
  if (!g_hash_table_lookup_extended(
          entityProperties->componentAuthority, componentTypeId, /* origKey */ NULL, &value)) {
    return false;
  }
  int64_t authoritativeClientId = POINTER_TO_CLIENT_ID(value);

  if (authoritativeClientId != clientId) {
    return false;
  }

  g_hash_table_remove(entityProperties->componentAuthority, componentTypeId);

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      g_hash_table_lookup(clientProperties->entityProperties, &entityId);
  assert(clientEntityProperties != NULL);
  assert(g_hash_table_contains(clientEntityProperties->authoritativeComponents, componentTypeId));
  g_hash_table_remove(clientEntityProperties->authoritativeComponents, componentTypeId);

  return true;
}

void shovelerClientPropertyManagerGetClientAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    GArray* outputArray) {
  g_array_set_size(outputArray, 0);

  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return;
  }

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      g_hash_table_lookup(clientProperties->entityProperties, &entityId);
  if (clientEntityProperties == NULL) {
    return;
  }

  GHashTableIter iter;
  const char* componentTypeId;
  g_hash_table_iter_init(&iter, clientEntityProperties->authoritativeComponents);
  while (g_hash_table_iter_next(&iter, (gpointer*) &componentTypeId, /* value */ NULL)) {
    g_array_append_val(outputArray, componentTypeId);
  }
}

bool shovelerClientPropertyManagerGetComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    long long int entityId,
    const char* componentTypeId,
    int64_t* outputClientId) {
  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      g_hash_table_lookup(clientPropertyManager->entityProperties, &entityId);
  if (entityProperties == NULL) {
    return false;
  }

  gpointer value;
  if (!g_hash_table_lookup_extended(
          entityProperties->componentAuthority, componentTypeId, /* origKey */ NULL, &value)) {
    return false;
  }

  *outputClientId = POINTER_TO_CLIENT_ID(value);
  return true;
}

bool shovelerClientPropertyManagerHasComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId) {
  int64_t authoritativeComponentId;
  if (!shovelerClientPropertyManagerGetComponentAuthority(
          clientPropertyManager, entityId, componentTypeId, &authoritativeComponentId)) {
    return false;
  }

  return authoritativeComponentId == clientId;
}

bool shovelerClientPropertyManagerAddComponentActivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      createOrGetClientEntityProperties(clientProperties, entityId);
  if (g_hash_table_contains(clientEntityProperties->activatedComponents, componentTypeId)) {
    return false;
  }

  g_hash_table_add(clientEntityProperties->activatedComponents, (gpointer) componentTypeId);
  return true;
}

bool shovelerClientPropertyManagerRemoveComponentActivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      g_hash_table_lookup(clientProperties->entityProperties, &entityId);
  if (clientEntityProperties == NULL) {
    return false;
  }

  if (!g_hash_table_contains(clientEntityProperties->activatedComponents, componentTypeId)) {
    return false;
  }

  g_hash_table_remove(clientEntityProperties->activatedComponents, (gpointer) componentTypeId);
  return true;
}

void shovelerClientPropertyManagerGetClientActivations(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    GArray* outputArray) {
  g_array_set_size(outputArray, 0);

  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return;
  }

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      g_hash_table_lookup(clientProperties->entityProperties, &entityId);
  if (clientEntityProperties == NULL) {
    return;
  }

  GHashTableIter iter;
  const char* componentTypeId;
  g_hash_table_iter_init(&iter, clientEntityProperties->activatedComponents);
  while (g_hash_table_iter_next(&iter, (gpointer*) &componentTypeId, /* value */ NULL)) {
    g_array_append_val(outputArray, componentTypeId);
  }
}

bool shovelerClientPropertyManagerGetComponentActivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId) {
  ShovelerClientPropertyManagerClientProperties* clientProperties =
      g_hash_table_lookup(clientPropertyManager->clientProperties, &clientId);
  if (clientProperties == NULL) {
    return false;
  }

  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      g_hash_table_lookup(clientProperties->entityProperties, &entityId);
  if (clientEntityProperties == NULL) {
    return false;
  }

  return g_hash_table_contains(clientEntityProperties->activatedComponents, componentTypeId);
}

static ShovelerClientPropertyManagerClientEntityProperties* createOrGetClientEntityProperties(
    ShovelerClientPropertyManagerClientProperties* clientProperties, long long int entityId) {
  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      g_hash_table_lookup(clientProperties->entityProperties, &entityId);
  if (clientEntityProperties == NULL) {
    clientEntityProperties = malloc(sizeof(ShovelerClientPropertyManagerClientEntityProperties));
    clientEntityProperties->entityId = entityId;
    clientEntityProperties->authoritativeComponents =
        g_hash_table_new(g_direct_hash, g_direct_equal);
    clientEntityProperties->activatedComponents = g_hash_table_new(g_direct_hash, g_direct_equal);
    g_hash_table_insert(
        clientProperties->entityProperties,
        &clientEntityProperties->entityId,
        clientEntityProperties);
  }
  return clientEntityProperties;
}

static ShovelerClientPropertyManagerEntityProperties* createOrGetEntityProperties(
    ShovelerClientPropertyManager* clientPropertyManager, long long int entityId) {
  ShovelerClientPropertyManagerEntityProperties* entityProperties =
      g_hash_table_lookup(clientPropertyManager->entityProperties, &entityId);
  if (entityProperties == NULL) {
    entityProperties = malloc(sizeof(ShovelerClientPropertyManagerEntityProperties));
    entityProperties->entityId = entityId;
    entityProperties->interestedClients = g_hash_table_new(g_direct_hash, g_direct_equal);
    entityProperties->componentAuthority = g_hash_table_new(g_direct_hash, g_direct_equal);
    g_hash_table_insert(
        clientPropertyManager->entityProperties, &entityProperties->entityId, entityProperties);
  }
  return entityProperties;
}

static void freeClientEntityProperties(void* clientEntityPropertiesPointer) {
  ShovelerClientPropertyManagerClientEntityProperties* clientEntityProperties =
      clientEntityPropertiesPointer;
  g_hash_table_destroy(clientEntityProperties->activatedComponents);
  g_hash_table_destroy(clientEntityProperties->authoritativeComponents);
  free(clientEntityProperties);
}

static void freeClientProperties(void* clientPropertiesPointer) {
  ShovelerClientPropertyManagerClientProperties* clientProperties = clientPropertiesPointer;
  g_hash_table_destroy(clientProperties->entityProperties);
  g_hash_table_destroy(clientProperties->interestedEntities);
  free(clientProperties);
}

static void freeEntityProperties(void* entityPropertiesPointer) {
  ShovelerClientPropertyManagerEntityProperties* entityProperties = entityPropertiesPointer;
  g_hash_table_destroy(entityProperties->componentAuthority);
  g_hash_table_destroy(entityProperties->interestedClients);
  free(entityProperties);
}
