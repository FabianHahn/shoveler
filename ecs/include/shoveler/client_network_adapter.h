#ifndef SHOVELER_CLIENT_NETWORK_ADAPTER_H
#define SHOVELER_CLIENT_NETWORK_ADAPTER_H

#include <glib.h>
#include <stdbool.h>

typedef enum {
  SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_NONE,
  SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED,
  SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED,
  SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE,
} ShovelerClientNetworkAdapterEventType;

typedef struct ShovelerClientNetworkAdapterEventStruct {
  ShovelerClientNetworkAdapterEventType type;
  /** for disconnect, contains the reason; for message, contains the message data */
  GString* payload;
} ShovelerClientNetworkAdapterEvent;

typedef struct ShovelerClientNetworkAdapterStruct {
  /** Send a message to the client with the specified handle. */
  bool (*sendMessage)(const unsigned char* data, int size, void* userData);
  /** Receives an incoming event, returning true if there was one (and there might be more). */
  bool (*receiveEvent)(
      bool (*receiveEventCallback)(const ShovelerClientNetworkAdapterEvent* event, void* userData),
      void* callbackUserData,
      void* userData);

  void* userData;
} ShovelerClientNetworkAdapter;

ShovelerClientNetworkAdapterEvent shovelerClientNetworkAdapterEventClientConnected();
ShovelerClientNetworkAdapterEvent shovelerClientNetworkAdapterEventClientDisconnected(
    const char* reason);
ShovelerClientNetworkAdapterEvent shovelerClientNetworkAdapterEventMessage(
    const unsigned char* data, int size);
/** Result must be freed by the caller. */
char* shovelerClientNetworkAdapterEventDebugPrint(const ShovelerClientNetworkAdapterEvent* event);
void shovelerClientNetworkAdapterEventClear(ShovelerClientNetworkAdapterEvent* event);

#endif