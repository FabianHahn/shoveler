#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "server_network_adapter_event_wrapper.h"
#include "server_op_wrapper.h"
#include <deque>
#include <string>

extern "C" {
#include <shoveler/client_connection_manager.h>
#include <shoveler/client_op.h>
#include <shoveler/component_type_indexer.h>
#include <shoveler/server_network_adapter.h>
}

using ::testing::AnyOf;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Matcher;
using ::testing::SizeIs;

namespace {
const long long int testEntityId = 42;
void* testClientHandle1 = (void*) "client_1";
void* testClientHandle2 = (void*) "client_2";
const auto* testDisconnectReason = "boom";

bool sendMessage(void* clientHandle, const unsigned char* data, int size, void* userData);
bool receiveEvent(
    bool (*receiveEventCallback)(const ShovelerServerNetworkAdapterEvent* event, void* userData),
    void* callbackUserData,
    void* userData);
void onClientConnected(
    ShovelerClientConnectionManager* clientConnectionManager, int64_t clientId, void* userData);
void onClientDisconnected(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const char* reason,
    void* userData);
void onReceiveServerOp(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const ShovelerServerOp* serverOp,
    void* userData);

class ShovelerClientConnectionManagerTest : public ::testing::Test {
public:
  virtual void SetUp() {
    componentTypeIndexer = shovelerComponentTypeIndexerCreate();

    networkAdapter.sendMessage = sendMessage;
    networkAdapter.receiveEvent = receiveEvent;
    networkAdapter.userData = this;

    callbacks.onClientConnected = onClientConnected;
    callbacks.onClientDisconnected = onClientDisconnected;
    callbacks.onReceiveServerOp = onReceiveServerOp;
    callbacks.userData = this;

    clientConnectionManager =
        shovelerClientConnectionManagerCreate(componentTypeIndexer, &networkAdapter, &callbacks);
  }

  virtual void TearDown() {
    shovelerComponentTypeIndexerFree(componentTypeIndexer);
    shovelerClientConnectionManagerFree(clientConnectionManager);
  }

  bool ConnectClient(void* clientHandle) {
    incomingServerEvents.push_back(
        ShovelerServerNetworkAdapterEventWrapper::ClientConnected(clientHandle));
    int eventsProcessed = shovelerClientConnectionManagerUpdate(clientConnectionManager);
    return eventsProcessed == 1;
  }

  bool DisconnectClient(void* clientHandle, const char* reason) {
    incomingServerEvents.push_back(
        ShovelerServerNetworkAdapterEventWrapper::ClientDisconnected(clientHandle, reason));
    int eventsProcessed = shovelerClientConnectionManagerUpdate(clientConnectionManager);
    return eventsProcessed == 1;
  }

  bool ReceiveMessage(void* clientHandle, GString* message) {
    incomingServerEvents.push_back(
        ShovelerServerNetworkAdapterEventWrapper::Message(clientHandle, message));
    int eventsProcessed = shovelerClientConnectionManagerUpdate(clientConnectionManager);
    return eventsProcessed == 1;
  }

  ShovelerComponentTypeIndexer* componentTypeIndexer;
  ShovelerServerNetworkAdapter networkAdapter;
  ShovelerClientConnectionManagerCallbacks callbacks;
  ShovelerClientConnectionManager* clientConnectionManager;

  std::deque<ShovelerServerNetworkAdapterEventWrapper> incomingServerEvents;

  struct SendMessageCall {
    void* clientHandle;
    std::string data;
  };
  std::vector<SendMessageCall> sendMessageCalls;

  std::vector<int64_t> clientConnectedCalls;

  struct ClientDisconnectedCall {
    int64_t clientId;
    std::string reason;

    bool operator==(const ClientDisconnectedCall& other) const {
      return clientId == other.clientId && reason == other.reason;
    };
  };
  std::vector<ClientDisconnectedCall> clientDisconnectedCalls;

  struct ReceiveServerOpCall {
    int64_t clientId;
    ShovelerServerOpWrapper serverOp;
  };
  std::vector<ReceiveServerOpCall> receiveServerOpCalls;
};

bool sendMessage(void* clientHandle, const unsigned char* data, int size, void* testPointer) {
  auto* test = static_cast<ShovelerClientConnectionManagerTest*>(testPointer);
  test->sendMessageCalls.emplace_back(ShovelerClientConnectionManagerTest::SendMessageCall{
      clientHandle, std::string((char*) data, size)});
  return true;
}

bool receiveEvent(
    bool (*receiveEventCallback)(const ShovelerServerNetworkAdapterEvent* event, void* userData),
    void* callbackUserData,
    void* testPointer) {
  auto* test = static_cast<ShovelerClientConnectionManagerTest*>(testPointer);
  if (test->incomingServerEvents.empty()) {
    return false;
  }

  bool result = receiveEventCallback(&*test->incomingServerEvents.front(), callbackUserData);
  test->incomingServerEvents.pop_front();
  return result;
}

void onClientConnected(
    ShovelerClientConnectionManager* clientConnectionManager, int64_t clientId, void* testPointer) {
  auto* test = static_cast<ShovelerClientConnectionManagerTest*>(testPointer);
  test->clientConnectedCalls.emplace_back(clientId);
}

void onClientDisconnected(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const char* reason,
    void* testPointer) {
  auto* test = static_cast<ShovelerClientConnectionManagerTest*>(testPointer);
  test->clientDisconnectedCalls.emplace_back(
      ShovelerClientConnectionManagerTest::ClientDisconnectedCall{clientId, reason});
}

void onReceiveServerOp(
    ShovelerClientConnectionManager* clientConnectionManager,
    int64_t clientId,
    const ShovelerServerOp* serverOp,
    void* testPointer) {
  auto* test = static_cast<ShovelerClientConnectionManagerTest*>(testPointer);
  test->receiveServerOpCalls.push_back(
      ShovelerClientConnectionManagerTest::ReceiveServerOpCall{clientId, serverOp});
}

auto IsSendMessageCall(Matcher<void*> clientHandle, Matcher<std::string> data) {
  return AllOf(
      Field(&ShovelerClientConnectionManagerTest::SendMessageCall::clientHandle, clientHandle),
      Field(&ShovelerClientConnectionManagerTest::SendMessageCall::data, data));
}

auto IsReceiveServerOpCall(Matcher<int64_t> clientId, Matcher<ShovelerServerOpWrapper> serverOp) {
  return AllOf(
      Field(&ShovelerClientConnectionManagerTest::ReceiveServerOpCall::clientId, clientId),
      Field(&ShovelerClientConnectionManagerTest::ReceiveServerOpCall::serverOp, serverOp));
}

MATCHER_P2(IsSerializedAddEntityClientOp, componentTypeIndexer, entityId, "") {
  const std::string& call = arg;

  auto deleter = [](auto* op) { shovelerClientOpFreeWithData(op); };
  std::unique_ptr<ShovelerClientOpWithData, decltype(deleter)> deserializedOp{
      shovelerClientOpCreateWithData(/* inputClientOp */ nullptr), deleter};
  int readIndex = 0;
  bool deserialized = shovelerClientOpDeserialize(
      deserializedOp.get(),
      componentTypeIndexer,
      reinterpret_cast<const unsigned char*>(call.c_str()),
      static_cast<int>(call.size()),
      &readIndex);
  if (!deserialized) {
    return false;
  }

  return deserializedOp->op.type == SHOVELER_CLIENT_OP_ADD_ENTITY &&
      deserializedOp->op.addEntity.entityId == entityId;
}

} // namespace

TEST_F(ShovelerClientConnectionManagerTest, connectDisconnect) {
  bool connected = ConnectClient(testClientHandle1);
  ASSERT_TRUE(connected);
  ASSERT_THAT(clientConnectedCalls, SizeIs(1));
  bool connectedAgain = ConnectClient(testClientHandle1);
  ASSERT_FALSE(connectedAgain);

  bool disconnectedWithoutConnecting = DisconnectClient(testClientHandle2, testDisconnectReason);
  ASSERT_FALSE(disconnectedWithoutConnecting);
  bool disconnected = DisconnectClient(testClientHandle1, testDisconnectReason);
  ASSERT_TRUE(disconnected);
  ASSERT_THAT(
      clientDisconnectedCalls,
      ElementsAre(ClientDisconnectedCall{clientConnectedCalls[0], testDisconnectReason}));
}

TEST_F(ShovelerClientConnectionManagerTest, receiveMessage) {
  ConnectClient(testClientHandle1);
  ASSERT_THAT(clientConnectedCalls, SizeIs(1));

  ShovelerServerOp serverOp = shovelerServerOp();
  serverOp.type = SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST;
  serverOp.addEntityInterest.entityId = testEntityId;

  auto deleter = [](GString* string) { g_string_free(string, /* freeSegment */ true); };
  std::unique_ptr<GString, decltype(deleter)> buffer{g_string_new(""), deleter};
  shovelerServerOpSerialize(&serverOp, componentTypeIndexer, buffer.get());
  bool received = ReceiveMessage(testClientHandle1, buffer.get());
  ASSERT_TRUE(received);
  ASSERT_THAT(
      receiveServerOpCalls,
      ElementsAre(
          IsReceiveServerOpCall(Eq(clientConnectedCalls[0]), IsAddEntityInterestOp(testEntityId))));

  receiveServerOpCalls.clear();
  bool receivedUnknown = ReceiveMessage(testClientHandle2, buffer.get());
  ASSERT_FALSE(receivedUnknown);
  ASSERT_THAT(receiveServerOpCalls, IsEmpty());
}

TEST_F(ShovelerClientConnectionManagerTest, sendClientOp) {
  ConnectClient(testClientHandle1);
  ASSERT_THAT(clientConnectedCalls, SizeIs(1));

  ShovelerClientOp clientOp = shovelerClientOp();
  clientOp.type = SHOVELER_CLIENT_OP_ADD_ENTITY;
  clientOp.addEntity.entityId = testEntityId;

  int64_t clients[] = {42, clientConnectedCalls[0], 1337}; // add two bogus client IDs
  int numSent = shovelerClientConnectionManagerSendClientOp(
      clientConnectionManager, clients, /* numClients */ 3, &clientOp);
  ASSERT_EQ(numSent, 1);
  ASSERT_THAT(
      sendMessageCalls,
      ElementsAre(IsSendMessageCall(
          Eq(testClientHandle1),
          IsSerializedAddEntityClientOp(componentTypeIndexer, testEntityId))));
}
