#ifndef SHOVELER_IN_MEMORY_NETWORK_ADAPTER_H
#define SHOVELER_IN_MEMORY_NETWORK_ADAPTER_H

#include <glib.h>
#include <shoveler/client_network_adapter.h>
#include <shoveler/server_network_adapter.h>

typedef struct ShovelerInMemoryNetworkAdapterStruct ShovelerInMemoryNetworkAdapter;

typedef struct ShovelerInMemoryNetworkAdapterClientStruct {
  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter;
  ShovelerClientNetworkAdapter clientNetworkAdapter;
  bool isDisconnecting;
  /** array of ShovelerClientNetworkAdapterEvent messages */
  GArray* clientEvents;
  int nextEventIndex;
} ShovelerInMemoryNetworkAdapterClient;

typedef struct ShovelerInMemoryNetworkAdapterStruct {
  /** set of ShovelerInMemoryNetworkAdapterClient */
  GHashTable* clients;
  /** array of ShovelerServerNetworkAdapterEvent messages */
  GArray* serverEvents;
  int nextEventIndex;
  ShovelerServerNetworkAdapter serverNetworkAdapter;
} ShovelerInMemoryNetworkAdapter;

ShovelerInMemoryNetworkAdapter* shovelerInMemoryNetworkAdapterCreate();
void shovelerInMemoryNetworkAdapterFree(ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter);
ShovelerServerNetworkAdapter* shovelerInMemoryNetworkAdapterGetServer(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter);
void* shovelerInMemoryNetworkAdapterConnectClient(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter);
bool shovelerInMemoryNetworkAdapterDisconnectClient(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter, void* clientHandle, const char* reason);
ShovelerClientNetworkAdapter* shovelerInMemoryNetworkAdapterGetClient(
    ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter, void* clientHandle);

#endif