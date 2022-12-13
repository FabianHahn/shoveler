#include "shoveler/server_network_adapter.h"

#include <string.h>

ShovelerServerNetworkAdapterEvent shovelerServerNetworkAdapterEventClientConnected(
    void* clientHandle) {
  ShovelerServerNetworkAdapterEvent event;
  event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED;
  event.clientHandle = clientHandle;
  return event;
}

ShovelerServerNetworkAdapterEvent shovelerServerNetworkAdapterEventClientDisconnected(
    void* clientHandle, const char* reason) {
  ShovelerServerNetworkAdapterEvent event;
  event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED;
  event.clientHandle = clientHandle;
  event.payload = g_string_new(reason);
  return event;
}

ShovelerServerNetworkAdapterEvent shovelerServerNetworkAdapterEventMessage(
    void* clientHandle, const unsigned char* data, int size) {
  ShovelerServerNetworkAdapterEvent event;
  event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE;
  event.clientHandle = clientHandle;
  event.payload = g_string_new("");
  g_string_append_len(event.payload, (gchar*) data, size);

  return event;
}

char* shovelerServerNetworkAdapterEventDebugPrint(const ShovelerServerNetworkAdapterEvent* event) {
  GString* output = g_string_new("");

  switch (event->type) {
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_NONE:
    g_string_append(output, "None()");
    break;
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
    g_string_append_printf(output, "ClientConnected(%p)", event->clientHandle);
    break;
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
    g_string_append_printf(
        output, "ClientDisconnected(%p, \"%s\")", event->clientHandle, event->payload->str);
    break;
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE:
    g_string_append_printf(
        output, "Message(%p, %d bytes)", event->clientHandle, event->payload->len);
    break;
  }

  return g_string_free(output, /* freeSegment */ false);
}

void shovelerServerNetworkAdapterEventClear(ShovelerServerNetworkAdapterEvent* event) {
  switch (event->type) {
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_NONE:
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
    break;
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
  case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE:
    g_string_free(event->payload, /* freeSegment */ true);
    break;
  }

  memset(event, 0, sizeof(ShovelerServerNetworkAdapterEvent));
}
