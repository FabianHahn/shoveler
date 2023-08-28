/**
 * The ClientPropertyManager stores the server-side state of all clients with respect to how they
 * see the world.
 *
 * For each client, it stores:
 *  - What entities they are interested in.
 *  - For each entity, which components they have authority over.
 *  - For each entity, which components have been server side deactivated. By default, components
 *    a client is interested in get activated automatically.
 *
 * For efficient lookups, the client property manager also stores entity keyed lookup tables to
 * quickly determine the set of interested clients for an entity and which client is authoritative
 * for which component.
 *
 * Note that the client property manager generally only stores state about the clients, but not the
 * world. This means that interest or authority can be granted to clients without the respective
 * entities and/or components existing in the world (yet). It is the ClientOpEmitter's job to query
 * the client property manager on world changes to determine whether pre-existing authority or
 * interest would lead to ops being emitted due to chose changes.
 */

#ifndef SHOVELER_CLIENT_PROPERTY_MANAGER_H
#define SHOVELER_CLIENT_PROPERTY_MANAGER_H

#include <glib.h>
#include <stdbool.h>
#include <stdint.h>

typedef void* ShovelerClientPropertyManagerClientDeactivations;

typedef struct ShovelerClientPropertyManagerClientEntityPropertiesStruct {
  long long int entityId;
  /** set of component type IDs */
  GHashTable* authoritativeComponents;
  /** set of component type IDs */
  GHashTable* deactivatedComponents;
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
/**
 * Returns the client IDs interested in a given entity.
 *
 * If componentTypeId is specified, excludes the authoritative client for that component.
 */
void shovelerClientPropertyManagerGetEntityInterest(
    ShovelerClientPropertyManager* clientPropertyManager,
    long long int entityId,
    const char* componentTypeId,
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

bool shovelerClientPropertyManagerAddComponentDeactivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);
bool shovelerClientPropertyManagerRemoveComponentDeactivation(
    ShovelerClientPropertyManager* clientPropertyManager,
    int64_t clientId,
    long long int entityId,
    const char* componentTypeId);
/** Returns the deactivated component type IDs on the specified entity for the given client. */
ShovelerClientPropertyManagerClientDeactivations*
shovelerClientPropertyManagerGetClientDeactivations(
    ShovelerClientPropertyManager* clientPropertyManager, int64_t clientId, long long int entityId);
void shovelerClientPropertyManagerClientDeactivationsGetAll(
    ShovelerClientPropertyManagerClientDeactivations* clientDeactivations, GArray* outputArray);
bool shovelerClientPropertyManagerClientDeactivationsGet(
    ShovelerClientPropertyManagerClientDeactivations* clientDeactivations,
    const char* componentTypeId);

#endif