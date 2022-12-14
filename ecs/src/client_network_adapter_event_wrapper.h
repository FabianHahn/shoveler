#ifndef SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_WRAPPER_H
#define SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_WRAPPER_H

#include "client_op_wrapper.h"

extern "C" {
#include <shoveler/client_network_adapter.h>
};

// Helper type to allow using ShovelerClientNetworkAdapterEvent object safely from C++.
struct ShovelerClientNetworkAdapterEventWrapper {
  static ShovelerClientNetworkAdapterEventWrapper ClientConnected() {
    ShovelerClientNetworkAdapterEventWrapper eventWrapper;
    eventWrapper.event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED;
    return eventWrapper;
  }

  static ShovelerClientNetworkAdapterEventWrapper ClientDisconnected(const char* reason) {
    ShovelerClientNetworkAdapterEventWrapper eventWrapper;
    eventWrapper.event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED;
    eventWrapper.event.payload = g_string_new(reason);
    return eventWrapper;
  }

  static ShovelerClientNetworkAdapterEventWrapper Message(const GString* data) {
    ShovelerClientNetworkAdapterEventWrapper eventWrapper;
    eventWrapper.event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE;
    eventWrapper.event.payload = g_string_new("");
    g_string_append_len(eventWrapper.event.payload, data->str, static_cast<gssize>(data->len));
    return eventWrapper;
  }

  ShovelerClientNetworkAdapterEventWrapper() {
    event.type = SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_NONE;
    event.payload = nullptr;
  }

  ShovelerClientNetworkAdapterEventWrapper(const ShovelerClientNetworkAdapterEvent* input)
      : ShovelerClientNetworkAdapterEventWrapper() {
    Assign(input);
  }

  ShovelerClientNetworkAdapterEventWrapper(const ShovelerClientNetworkAdapterEventWrapper& other)
      : ShovelerClientNetworkAdapterEventWrapper(&*other) {}

  ~ShovelerClientNetworkAdapterEventWrapper() { shovelerClientNetworkAdapterEventClear(&event); }

  ShovelerClientNetworkAdapterEventWrapper& operator=(
      const ShovelerClientNetworkAdapterEventWrapper& other) {
    if (&other != this) {
      Assign(&*other);
    }

    return *this;
  }

  const ShovelerClientNetworkAdapterEvent& operator*() const { return event; }

  const ShovelerClientNetworkAdapterEvent* operator->() const { return &event; }

  void Assign(const ShovelerClientNetworkAdapterEvent* input) {
    event.type = input->type;

    if (event.payload != nullptr) {
      g_string_free(event.payload, /* freeSegment */ true);
      event.payload = nullptr;
    }
    switch (input->type) {
    case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_NONE:
    case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
      break;
    case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
      event.payload = g_string_new(input->payload->str);
      break;
    case SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE:
      event.payload = g_string_new("");
      g_string_append_len(
          event.payload, input->payload->str, static_cast<gssize>(input->payload->len));
      break;
    }
  }

private:
  ShovelerClientNetworkAdapterEvent event;
};

inline std::ostream& operator<<(
    std::ostream& output, const ShovelerClientNetworkAdapterEventWrapper& event) {
  char* debugOutput = shovelerClientNetworkAdapterEventDebugPrint(&*event);
  output << debugOutput;
  free(debugOutput);
  return output;
}

MATCHER(IsClientConnectedClientEvent, "") {
  const ShovelerClientNetworkAdapterEventWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED;
}

MATCHER_P(IsClientDisconnectedClientEvent, reason, "") {
  const ShovelerClientNetworkAdapterEventWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED &&
      std::string{call->payload->str, call->payload->len} == reason;
}

MATCHER_P(IsMessageClientEvent, message, "") {
  const ShovelerClientNetworkAdapterEventWrapper& call = arg;
  return call->type == SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE &&
      std::string{call->payload->str, call->payload->len} == message;
}

#endif
