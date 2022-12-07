#ifndef SHOVELER_CLIENT_PROPERTY_MANAGER_H
#define SHOVELER_CLIENT_PROPERTY_MANAGER_H

#include <glib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct ShovelerClientPropertyManagerClientEntityPropertiesStruct {
  long long int entityId;
  /** set of component type IDs */
  GHashTable* authoritativeComponents;
  /** set of component type IDs */
  GHashTable* activatedComponents;
} ShovelerClientPropertyManagerClientEntityProperties;

typedef struct ShovelerClientPropertyManagerClientPropertiesStruct {
  int64_t clientId;
  /** set of entity IDs */
  GHashTable* interestedEntities;
  /** map from entity ID to ShovelerClientPropertyManagerClientEntityProperties */
  GHashTable* entityProperties;
} ShovelerClientPropertyManagerClientProperties;

typedef struct ShovelerClientPropertyManagerEntityPropertiesStruct {
  long long int entityId;
  /** set of client IDs */
  GHashTable* interestedClients;
  /** map from component type Id to client ID */
  GHashTable* componentAuthority;
} ShovelerClientPropertyManagerEntityProperties;

typedef struct ShovelerClientPropertyManagerStruct {
  /** map from client ID to ShovelerClientPropertyManagerClientProperties */
  GHashTable* clientProperties;
  /** map from entity ID to ShovelerClientPropertyManagerEntityProperties */
  GHashTable* entityProperties;
} ShovelerClientPropertyManager;

/** Caller retains ownership over passed objects. */
ShovelerClientPropertyManager* shovelerClientPropertyManagerCreate();
void shovelerClientPropertyManagerFree(ShovelerClientPropertyManager* clientPropertyManager);

bool shovelerClientPropertyManagerAddClient(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId);
bool shovelerClientPropertyManagerRemoveClient(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId);

bool shovelerClientPropertyManagerAddEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId, long long int entityId);
bool shovelerClientPropertyManagerRemoveEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId, long long int entityId);
bool shovelerClientPropertyManagerHasEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId, long long int entityId);
/** Returns the client IDs interested in a given entity ID. */
void shovelerClientPropertyManagerGetEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager,
    long long int entityId,
    GArray* outputArray);

bool shovelerClientPropertyManagerAddComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);
bool shovelerClientPropertyManagerRemoveComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);
/** Returns the authoritative component type IDs on the specified entity for the given client. */
void shovelerClientPropertyManagerGetClientAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    GArray* outputArray);
bool shovelerClientPropertyManagerGetComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    long long int entityId,
    const char* componentTypeId,
    int64_t* outputClientId);
bool shovelerClientPropertyManagerHasComponentAuthority(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);

bool shovelerClientPropertyManagerAddComponentActivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);
bool shovelerClientPropertyManagerRemoveComponentActivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);
/** Returns the activated component type IDs on the specified entity for the given client. */
void shovelerClientPropertyManagerGetClientActivations(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    GArray* outputArray);
bool shovelerClientPropertyManagerGetComponentActivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);

#endif