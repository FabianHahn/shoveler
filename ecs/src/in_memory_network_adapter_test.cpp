#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "client_network_adapter_event_wrapper.h"
#include "server_network_adapter_event_wrapper.h"
#include <vector>

extern "C" {
#include <shoveler/in_memory_network_adapter.h>
}

using ::testing::ElementsAre;

namespace {
const auto* testDisconnectReason = "goodbye";
const std::string testMessage = "the bird is the word";

static bool receiveServerEvent(
    const ShovelerServerNetworkAdapterEvent* event, void* resultPointer) {
  auto& result =
      *static_cast<std::vector<ShovelerServerNetworkAdapterEventWrapper>*>(resultPointer);
  result.emplace_back(event);
  return true;
}

static bool receiveClientEvent(
    const ShovelerClientNetworkAdapterEvent* event, void* resultPointer) {
  auto& result =
      *static_cast<std::vector<ShovelerClientNetworkAdapterEventWrapper>*>(resultPointer);
  result.emplace_back(event);
  return true;
}

class ShovelerInMemoryNetworkAdapterTest : public ::testing::Test {
public:
  virtual void SetUp() {
    inMemoryNetworkAdapter = shovelerInMemoryNetworkAdapterCreate();
    server = shovelerInMemoryNetworkAdapterGetServer(inMemoryNetworkAdapter);
  }

  virtual void TearDown() { shovelerInMemoryNetworkAdapterFree(inMemoryNetworkAdapter); }

  std::vector<ShovelerServerNetworkAdapterEventWrapper> ReceiveServerEvents() {
    std::vector<ShovelerServerNetworkAdapterEventWrapper> result;
    while (server->receiveEvent(receiveServerEvent, &result, server->userData)) {
    }
    return result;
  }

  std::vector<ShovelerClientNetworkAdapterEventWrapper> ReceiveClientEvents(
      ShovelerClientNetworkAdapter* client) {
    std::vector<ShovelerClientNetworkAdapterEventWrapper> result;
    while (client->receiveEvent(receiveClientEvent, &result, client->userData)) {
    }
    return result;
  }

  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter;
  ShovelerServerNetworkAdapter* server;
};

} // namespace

TEST_F(ShovelerInMemoryNetworkAdapterTest, connectDisconnect) {
  void* clientHandle = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  ASSERT_NE(clientHandle, nullptr);
  ASSERT_THAT(ReceiveServerEvents(), ElementsAre(IsClientConnectedServerEvent(clientHandle)));

  auto* client = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle);
  ASSERT_THAT(ReceiveClientEvents(client), ElementsAre(IsClientConnectedClientEvent()));

  bool disconnecting = shovelerInMemoryNetworkAdapterDisconnectClient(
      inMemoryNetworkAdapter, clientHandle, testDisconnectReason);
  ASSERT_TRUE(disconnecting);
  ASSERT_THAT(
      ReceiveServerEvents(),
      ElementsAre(IsClientDisconnectedServerEvent(clientHandle, testDisconnectReason)));
  ASSERT_THAT(
      ReceiveClientEvents(client),
      ElementsAre(IsClientDisconnectedClientEvent(testDisconnectReason)));
}

TEST_F(ShovelerInMemoryNetworkAdapterTest, sendToServer) {
  void* clientHandle = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle);
  bool sent = client->sendMessage(
      reinterpret_cast<const unsigned char*>(testMessage.c_str()),
      static_cast<int>(testMessage.length()),
      client->userData);
  ASSERT_TRUE(sent);
  bool sentAgain = client->sendMessage(
      reinterpret_cast<const unsigned char*>(testMessage.c_str()),
      static_cast<int>(testMessage.length()),
      client->userData);
  ASSERT_TRUE(sentAgain);
  shovelerInMemoryNetworkAdapterDisconnectClient(
      inMemoryNetworkAdapter, clientHandle, testDisconnectReason);
  ASSERT_THAT(
      ReceiveServerEvents(),
      ElementsAre(
          IsClientConnectedServerEvent(clientHandle),
          IsMessageServerEvent(clientHandle, testMessage),
          IsMessageServerEvent(clientHandle, testMessage),
          IsClientDisconnectedServerEvent(clientHandle, testDisconnectReason)));
}

TEST_F(ShovelerInMemoryNetworkAdapterTest, sendToClient) {
  void* clientHandle1 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  void* clientHandle2 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client1 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle1);
  auto* client2 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle2);
  bool sent = server->sendMessage(
      clientHandle1,
      reinterpret_cast<const unsigned char*>(testMessage.c_str()),
      static_cast<int>(testMessage.length()),
      server->userData);
  ASSERT_TRUE(sent);
  bool sentAgain = server->sendMessage(
      clientHandle1,
      reinterpret_cast<const unsigned char*>(testMessage.c_str()),
      static_cast<int>(testMessage.length()),
      server->userData);
  ASSERT_TRUE(sentAgain);
  ASSERT_THAT(
      ReceiveClientEvents(client1),
      ElementsAre(
          IsClientConnectedClientEvent(),
          IsMessageClientEvent(testMessage),
          IsMessageClientEvent(testMessage)));
  ASSERT_THAT(ReceiveClientEvents(client2), ElementsAre(IsClientConnectedClientEvent()));
}
