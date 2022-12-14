#include "shoveler/client_network_adapter.h"

#include <string.h>

ShovelerClientNetworkAdapterEvent shovelerClientNetworkAdapterEventClientConnected() {
  ShovelerClientNetworkAdapterEvent event;
  event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED;
  return event;
}

ShovelerClientNetworkAdapterEvent shovelerClientNetworkAdapterEventClientDisconnected(
    const char* reason) {
  ShovelerClientNetworkAdapterEvent event;
  event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED;
  event.payload = g_string_new(reason);
  return event;
}

ShovelerClientNetworkAdapterEvent shovelerClientNetworkAdapterEventMessage(
    const unsigned char* data, int size) {
  ShovelerClientNetworkAdapterEvent event;
  event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE;
  event.payload = g_string_new("");
  g_string_append_len(event.payload, (gchar*) data, size);

  return event;
}

char* shovelerClientNetworkAdapterEventDebugPrint(const ShovelerClientNetworkAdapterEvent* event) {
  GString* output = g_string_new("");

  switch (event->type) {
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_NONE:
    g_string_append(output, "None()");
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
    g_string_append(output, "ClientConnected()");
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
    g_string_append_printf(output, "ClientDisconnected(\"%s\")", event->payload->str);
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE:
    g_string_append_printf(output, "Message(%d bytes)", event->payload->len);
    break;
  }

  return g_string_free(output, /* freeSegment */ false);
}

void shovelerClientNetworkAdapterEventClear(ShovelerClientNetworkAdapterEvent* event) {
  switch (event->type) {
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_NONE:
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
    break;
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
  case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE:
    g_string_free(event->payload, /* freeSegment */ true);
    break;
  }

  memset(event, 0, sizeof(ShovelerClientNetworkAdapterEvent));
}
