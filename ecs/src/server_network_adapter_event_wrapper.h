#ifndef SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_WRAPPER_H
#define SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_WRAPPER_H

extern "C" {
#include <shoveler/server_network_adapter.h>
};

// Helper type to allow using ShovelerServerNetworkAdapterEvent object safely from C++.
struct ShovelerServerNetworkAdapterEventWrapper {
  static ShovelerServerNetworkAdapterEventWrapper ClientConnected(void* clientHandle) {
    ShovelerServerNetworkAdapterEventWrapper eventWrapper;
    eventWrapper.event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED;
    eventWrapper.event.clientHandle = clientHandle;
    return eventWrapper;
  }

  static ShovelerServerNetworkAdapterEventWrapper ClientDisconnected(
      void* clientHandle, const char* reason) {
    ShovelerServerNetworkAdapterEventWrapper eventWrapper;
    eventWrapper.event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED;
    eventWrapper.event.clientHandle = clientHandle;
    eventWrapper.event.payload = g_string_new(reason);
    return eventWrapper;
  }

  static ShovelerServerNetworkAdapterEventWrapper Message(void* clientHandle, const GString* data) {
    ShovelerServerNetworkAdapterEventWrapper eventWrapper;
    eventWrapper.event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE;
    eventWrapper.event.clientHandle = clientHandle;
    eventWrapper.event.payload = g_string_new("");
    g_string_append_len(eventWrapper.event.payload, data->str, static_cast<gssize>(data->len));
    return eventWrapper;
  }

  ShovelerServerNetworkAdapterEventWrapper() {
    event.type = SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_NONE;
  }

  ShovelerServerNetworkAdapterEventWrapper(const ShovelerServerNetworkAdapterEvent* input) {
    Assign(input);
  }

  ShovelerServerNetworkAdapterEventWrapper(const ShovelerServerNetworkAdapterEventWrapper& other)
      : ShovelerServerNetworkAdapterEventWrapper(&*other) {}

  ~ShovelerServerNetworkAdapterEventWrapper() { shovelerServerNetworkAdapterEventClear(&event); }

  ShovelerServerNetworkAdapterEventWrapper& operator=(
      const ShovelerServerNetworkAdapterEventWrapper& other) {
    if (&other != this) {
      Assign(&*other);
    }

    return *this;
  }

  const ShovelerServerNetworkAdapterEvent& operator*() const { return event; }

  const ShovelerServerNetworkAdapterEvent* operator->() const { return &event; }

private:
  void Assign(const ShovelerServerNetworkAdapterEvent* input) {
    event.type = input->type;
    switch (input->type) {
    case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_NONE:
      break;
    case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED:
      event.clientHandle = input->clientHandle;
      break;
    case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED:
      event.clientHandle = input->clientHandle;
      event.payload = g_string_new(input->payload->str);
      break;
    case SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE:
      event.clientHandle = input->clientHandle;
      event.payload = g_string_new("");
      g_string_append_len(
          event.payload, input->payload->str, static_cast<gssize>(input->payload->len));
      break;
    }
  }

  ShovelerServerNetworkAdapterEvent event;
};

inline std::ostream& operator<<(
    std::ostream& output, const ShovelerServerNetworkAdapterEventWrapper& event) {
  char* debugOutput = shovelerServerNetworkAdapterEventDebugPrint(&*event);
  output << debugOutput;
  free(debugOutput);
  return output;
}

MATCHER_P(IsClientConnectedServerEvent, clientHandle, "") {
  const ShovelerServerNetworkAdapterEventWrapper& call = arg;
  return call->type == SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_CONNECTED &&
      call->clientHandle == clientHandle;
}

MATCHER_P2(IsClientDisconnectedServerEvent, clientHandle, reason, "") {
  const ShovelerServerNetworkAdapterEventWrapper& call = arg;
  return call->type == SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_CLIENT_DISCONNECTED &&
      call->clientHandle == clientHandle &&
      std::string{call->payload->str, call->payload->len} == reason;
}

MATCHER_P2(IsMessageServerEvent, clientHandle, message, "") {
  const ShovelerServerNetworkAdapterEventWrapper& call = arg;
  return call->type == SHOVELER_SERVER_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE &&
      call->clientHandle == clientHandle &&
      std::string{call->payload->str, call->payload->len} == message;
}

#endif
