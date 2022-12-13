#ifndef SHOVELER_SERVER_NETWORK_ADAPTER_H
#define SHOVELER_SERVER_NETWORK_ADAPTER_H

#include <glib.h>
#include <stdbool.h>

typedef enum {
  SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_NONE,
  SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED,
  SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED,
  SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE,
} ShovelerServerNetworkAdapterEventType;

typedef struct ShovelerServerNetworkAdapterEventStruct {
  ShovelerServerNetworkAdapterEventType type;
  void* clientHandle;
  /** for disconnect, contains the reason; for message, contains the message data */
  GString* payload;
} ShovelerServerNetworkAdapterEvent;

typedef struct ShovelerServerNetworkAdapterStruct {
  /** Send a message to the client with the specified handle. */
  bool (*sendMessage)(void* clientHandle, const unsigned char* data, int size, void* userData);
  /** Receives an incoming event, returning true if there was one (and there might be more). */
  bool (*receiveEvent)(
      bool (*receiveEventCallback)(const ShovelerServerNetworkAdapterEvent* event, void* userData),
      void* callbackUserData,
      void* userData);

  void* userData;
} ShovelerServerNetworkAdapter;

ShovelerServerNetworkAdapterEvent shovelerServerNetworkAdapterEventClientConnected(
    void* clientHandle);
ShovelerServerNetworkAdapterEvent shovelerServerNetworkAdapterEventClientDisconnected(
    void* clientHandle, const char* reason);
ShovelerServerNetworkAdapterEvent shovelerServerNetworkAdapterEventMessage(
    void* clientHandle, const unsigned char* data, int size);
/** Result must be freed by the caller. */
char* shovelerServerNetworkAdapterEventDebugPrint(const ShovelerServerNetworkAdapterEvent* event);
void shovelerServerNetworkAdapterEventClear(ShovelerServerNetworkAdapterEvent* event);

#endif