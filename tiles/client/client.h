/**
 * A tiles client:
 *  - populates a client side world view according to a network connection
 *  - receives player input and communicated it to the server
 *  - manages interest for the client according to its position in the world
 */

#ifndef SHOVELER_TILES_CLIENT_H
#define SHOVELER_TILES_CLIENT_H

#include <glib.h>
#include <shoveler/client_world_updater.h>
#include <stdbool.h>

typedef struct ShovelerClientNetworkAdapterStruct ShovelerClientNetworkAdapter;
typedef struct ShovelerWorldStruct ShovelerWorld;
typedef struct ShovelerComponentTypeIndexerStruct ShovelerComponentTypeIndexer;
typedef struct ShovelerClientOpWithDataStruct ShovelerClientOpWithData;

typedef struct ShovelerTilesClientStruct {
  ShovelerClientNetworkAdapter* clientNetworkAdapter;
  ShovelerComponentTypeIndexer* componentTypeIndexer;
  ShovelerWorld* world;
  ShovelerClientWorldUpdater clientWorldUpdater;
  ShovelerClientOpWithData* deserializedOp;
  GString* serializedOp;
} ShovelerTilesClient;

ShovelerTilesClient* shovelerTilesClientCreate(
    /** Client side network connection to the game server */
    ShovelerClientNetworkAdapter* clientNetworkAdapter,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    /** World to populate */
    ShovelerWorld* world);
void shovelerTilesClientUpdate(ShovelerTilesClient* tilesClient);
bool shovelerTilesClientUpdateSendAddInterest(
    ShovelerTilesClient* tilesClient, long long int entityId);
void shovelerTilesClientFree(ShovelerTilesClient* tilesClient);

#endif
