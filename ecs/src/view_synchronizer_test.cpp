#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "client_network_adapter_event_wrapper.h"
#include <vector>

extern "C" {
#include <shoveler/component.h>
#include <shoveler/in_memory_network_adapter.h>
#include <shoveler/schema.h>
#include <shoveler/server_op.h>
#include <shoveler/system.h>
#include <shoveler/view_synchronizer.h>

#include "test_component_types.h"
}

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;

namespace {
const long long int testEntityId1 = 1;
const long long int testEntityId2 = 2;
const long long int testEntityId3 = 3;
const long long int testEntityId4 = 4;
const long long int testEntityId5 = 5;
const auto* testDisconnectReason = "goodbye";

void onClientConnected(
    ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* userData);
void onClientDisconnected(
    ShovelerViewSynchronizer* viewSynchronizer,
    int64_t clientId,
    const char* reason,
    void* userData);

static bool receiveClientEvent(
    const ShovelerClientNetworkAdapterEvent* event, void* resultPointer) {
  auto& result = *static_cast<ShovelerClientNetworkAdapterEventWrapper*>(resultPointer);
  result.Assign(event);
  return true;
}

class ShovelerViewSynchronizerTest : public ::testing::Test {
public:
  virtual void SetUp() {
    inMemoryNetworkAdapter = shovelerInMemoryNetworkAdapterCreate();

    schema = shovelerSchemaCreate();
    ShovelerComponentType* componentType1 = shovelerCreateTestComponentType1();
    ShovelerComponentType* componentType2 = shovelerCreateTestComponentType2();
    ShovelerComponentType* componentType3 = shovelerCreateTestComponentType3();
    shovelerSchemaAddComponentType(schema, componentType1);
    shovelerSchemaAddComponentType(schema, componentType2);
    shovelerSchemaAddComponentType(schema, componentType3);

    system = shovelerSystemCreate();

    callbacks.onClientConnected = onClientConnected;
    callbacks.onClientDisconnected = onClientDisconnected;
    callbacks.userData = this;

    ShovelerServerNetworkAdapter* serverNetworkAdapter =
        shovelerInMemoryNetworkAdapterGetServer(inMemoryNetworkAdapter);
    viewSynchronizer =
        shovelerViewSynchronizerCreate(schema, system, serverNetworkAdapter, &callbacks);

    server = shovelerViewSynchronizerGetServerController(viewSynchronizer);
    world = shovelerViewSynchronizerGetWorld(viewSynchronizer);

    entity1 = shovelerWorldAddEntity(world, testEntityId1);
    entity1Component1 = shovelerWorldEntityAddComponent(entity1, componentType1Id);
    entity1Component2 = shovelerWorldEntityAddComponent(entity1, componentType2Id);
    entity2 = shovelerWorldAddEntity(world, testEntityId2);
    entity2Component1 = shovelerWorldEntityAddComponent(entity2, componentType1Id);
    entity3 = shovelerWorldAddEntity(world, testEntityId3);
    entity3Component3 = shovelerWorldEntityAddComponent(entity3, componentType3Id);
  }

  virtual void TearDown() {
    shovelerViewSynchronizerFree(viewSynchronizer);
    shovelerSystemFree(system);
    shovelerSchemaFree(schema);
    shovelerInMemoryNetworkAdapterFree(inMemoryNetworkAdapter);
  }

  bool SendAddEntityInterest(ShovelerClientNetworkAdapter* client, long long int entityId) {
    ShovelerServerOp serverOp = shovelerServerOp();
    serverOp.type = SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST;
    serverOp.addEntityInterest.entityId = entityId;
    return SendServerOp(client, &serverOp);
  }

  bool SendRemoveEntityInterest(ShovelerClientNetworkAdapter* client, long long int entityId) {
    ShovelerServerOp serverOp = shovelerServerOp();
    serverOp.type = SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST;
    serverOp.removeEntityInterest.entityId = entityId;
    return SendServerOp(client, &serverOp);
  }

  bool SendUpdateComponent(
      ShovelerClientNetworkAdapter* client,
      long long int entityId,
      const char* componentTypeId,
      int fieldId,
      const ShovelerComponentFieldValue* fieldValue) {
    ShovelerServerOp serverOp = shovelerServerOp();
    serverOp.type = SHOVELER_SERVER_OP_UPDATE_COMPONENT;
    serverOp.updateComponent.entityId = entityId;
    serverOp.updateComponent.componentTypeId = componentTypeId;
    serverOp.updateComponent.fieldId = fieldId;
    serverOp.updateComponent.fieldValue = fieldValue;
    return SendServerOp(client, &serverOp);
  }

  bool SendServerOp(ShovelerClientNetworkAdapter* client, const ShovelerServerOp* serverOp) {
    auto deleter = [](GString* string) { g_string_free(string, /* freeSegment */ true); };
    std::unique_ptr<GString, decltype(deleter)> buffer{g_string_new(""), deleter};
    if (!shovelerServerOpSerialize(
            serverOp, viewSynchronizer->componentTypeIndexer, buffer.get())) {
      return false;
    }

    return client->sendMessage(
        reinterpret_cast<const unsigned char*>(buffer->str),
        static_cast<int>(buffer->len),
        client->userData);
  }

  std::vector<ShovelerClientOpWrapper> ReceiveClientOps(
      ShovelerClientNetworkAdapter* client, int maxOps = -1) {
    std::vector<ShovelerClientOpWrapper> result;
    ShovelerClientNetworkAdapterEventWrapper event;
    int numOps = 0;
    while (client->receiveEvent(receiveClientEvent, &event, client->userData)) {
      if (event->type == SHOVELER_CLIENT_NETWORK_ADAPTER_EVENT_TYPE_MESSAGE) {
        ShovelerClientOpWrapper deserializedOp;
        int readIndex = 0;
        if (!shovelerClientOpDeserialize(
                deserializedOp.opWithData,
                viewSynchronizer->componentTypeIndexer,
                reinterpret_cast<const unsigned char*>(event->payload->str),
                static_cast<int>(event->payload->len),
                &readIndex)) {
          continue;
        }
        result.push_back(deserializedOp);

        numOps++;
        if (numOps == maxOps) {
          break;
        }
      }
    }
    return result;
  }

  ShovelerInMemoryNetworkAdapter* inMemoryNetworkAdapter;
  ShovelerSchema* schema;
  ShovelerSystem* system;
  ShovelerViewSynchronizerCallbacks callbacks;
  ShovelerViewSynchronizer* viewSynchronizer;
  ShovelerServerController* server;
  ShovelerWorld* world;

  ShovelerWorldEntity* entity1;
  ShovelerComponent* entity1Component1;
  ShovelerComponent* entity1Component2;
  ShovelerWorldEntity* entity2;
  ShovelerComponent* entity2Component1;
  ShovelerWorldEntity* entity3;
  ShovelerComponent* entity3Component3;

  std::vector<int64_t> clientConnectedCalls;
  std::vector<int64_t> clientDisconnectedCalls;
};

void onClientConnected(
    ShovelerViewSynchronizer* viewSynchronizer, int64_t clientId, void* testPointer) {
  auto* test = static_cast<ShovelerViewSynchronizerTest*>(testPointer);
  test->clientConnectedCalls.push_back(clientId);
}

void onClientDisconnected(
    ShovelerViewSynchronizer* viewSynchronizer,
    int64_t clientId,
    const char* reason,
    void* testPointer) {
  auto* test = static_cast<ShovelerViewSynchronizerTest*>(testPointer);
  test->clientDisconnectedCalls.push_back(clientId);
}

} // namespace

TEST_F(ShovelerViewSynchronizerTest, connectDisconnect) {
  void* clientHandle = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  ASSERT_NE(clientHandle, nullptr);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(clientConnectedCalls, SizeIs(1));

  bool disconnecting = shovelerInMemoryNetworkAdapterDisconnectClient(
      inMemoryNetworkAdapter, clientHandle, testDisconnectReason);
  ASSERT_TRUE(disconnecting);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(clientDisconnectedCalls, SizeIs(1));
}

TEST_F(ShovelerViewSynchronizerTest, checkoutUncheckout) {
  void* clientHandle = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle);

  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(clientConnectedCalls, SizeIs(1));
  auto clientId = clientConnectedCalls[0];

  // Predelegate components on the entity we are going to add interest to.
  bool delegated =
      shovelerServerControllerDelegateComponent(server, testEntityId1, componentType3Id, clientId);
  ASSERT_TRUE(delegated);

  // Add interest over preexisting entity and assure it gets checked out.
  bool addEntityInterestSent = SendAddEntityInterest(client, testEntityId1);
  ASSERT_TRUE(addEntityInterestSent);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(
      ReceiveClientOps(client, 2),
      ElementsAre(
          IsAddEntityOp(testEntityId1), IsDelegateComponentOp(testEntityId1, componentType3Id)));
  ASSERT_THAT(
      ReceiveClientOps(client, 6),
      UnorderedElementsAre(
          IsAddComponentOp(testEntityId1, componentType1Id),
          IsUpdateComponentOp(testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE),
          IsUpdateComponentOp(
              testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE),
          IsUpdateComponentOp(
              testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE),
          IsAddComponentOp(testEntityId1, componentType2Id),
          IsUpdateComponentOp(
              testEntityId1, componentType2Id, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE)));
  ASSERT_THAT(
      ReceiveClientOps(client),
      UnorderedElementsAre(
          IsActivateComponentOp(testEntityId1, componentType1Id),
          IsActivateComponentOp(testEntityId1, componentType2Id)));

  // Remove interest over the entity and assure it gets uncheckouted.
  bool removeEntityInterestSent = SendRemoveEntityInterest(client, testEntityId1);
  ASSERT_TRUE(removeEntityInterestSent);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(ReceiveClientOps(client), ElementsAre(IsRemoveEntityOp(testEntityId1)));
}

TEST_F(ShovelerViewSynchronizerTest, addRemove) {
  void* clientHandle1 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  void* clientHandle2 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client1 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle1);
  auto* client2 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle2);

  // Make client 1 interest in the entity we're going to create and modify, and client 2 in another
  // entity that will never exist.
  SendAddEntityInterest(client1, testEntityId4);
  SendAddEntityInterest(client2, testEntityId5);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  auto clientId1 = clientConnectedCalls[0];

  // Mess around with entity 4, and assure that client 1 sees it all while client 2 sees none of it.
  auto* entity4 = shovelerWorldAddEntity(world, testEntityId4);
  auto* entity4Component2 = shovelerWorldEntityAddComponent(entity4, componentType2Id);
  bool updated = shovelerComponentUpdateField(
      entity4Component2,
      COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
      &entity1Component2->fieldValues[COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE],
      /* isCanonical */ true);
  ASSERT_TRUE(updated);
  shovelerServerControllerDelegateComponent(server, testEntityId4, componentType2Id, clientId1);
  shovelerServerControllerUndelegateComponent(server, testEntityId4, componentType2Id, clientId1);
  shovelerServerControllerDeactivateComponent(server, testEntityId4, componentType2Id, clientId1);
  shovelerServerControllerUndeactivateComponent(server, testEntityId4, componentType2Id, clientId1);
  shovelerWorldEntityRemoveComponent(entity4, componentType2Id);
  shovelerWorldRemoveEntity(world, testEntityId4);
  ASSERT_THAT(
      ReceiveClientOps(client1),
      ElementsAre(
          IsAddEntityOp(testEntityId4),
          IsAddComponentOp(testEntityId4, componentType2Id),
          IsActivateComponentOp(testEntityId4, componentType2Id),
          IsUpdateComponentOp(
              testEntityId4, componentType2Id, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE),
          IsDelegateComponentOp(testEntityId4, componentType2Id),
          IsUndelegateComponentOp(testEntityId4, componentType2Id),
          IsDeactivateComponentOp(testEntityId4, componentType2Id),
          IsActivateComponentOp(testEntityId4, componentType2Id),

          IsRemoveComponentOp(testEntityId4, componentType2Id),
          IsRemoveEntityOp(testEntityId4)));
  ASSERT_THAT(ReceiveClientOps(client2), IsEmpty());
}

TEST_F(ShovelerViewSynchronizerTest, fanout) {
  void* clientHandle1 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  void* clientHandle2 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  void* clientHandle3 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client1 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle1);
  auto* client2 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle2);
  auto* client3 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle3);
  SendAddEntityInterest(client1, testEntityId1);
  SendAddEntityInterest(client1, testEntityId2);
  SendAddEntityInterest(client2, testEntityId1);
  shovelerViewSynchronizerUpdate(viewSynchronizer);

  // Flush received ops for all clients.
  ReceiveClientOps(client1);
  ReceiveClientOps(client2);
  ReceiveClientOps(client3);

  // Updating entity 1 should be fanned out to clients 1 and 2.
  shovelerComponentUpdateField(
      entity1Component1,
      COMPONENT_TYPE_1_FIELD_PRIMITIVE,
      &entity1Component1->fieldValues[COMPONENT_TYPE_1_FIELD_PRIMITIVE],
      /* isCanonical */ true);
  shovelerComponentUpdateField(
      entity1Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
      &entity1Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE],
      /* isCanonical */ true);
  shovelerComponentUpdateField(
      entity1Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE,
      &entity1Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE],
      /* isCanonical */ true);
  shovelerComponentUpdateField(
      entity1Component2,
      COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
      &entity1Component2->fieldValues[COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE],
      /* isCanonical */ true);

  // Updating entity 2 should only be fanned out to client 1.
  shovelerComponentUpdateField(
      entity2Component1,
      COMPONENT_TYPE_1_FIELD_PRIMITIVE,
      &entity2Component1->fieldValues[COMPONENT_TYPE_1_FIELD_PRIMITIVE],
      /* isCanonical */ true);
  shovelerComponentUpdateField(
      entity2Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
      &entity2Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE],
      /* isCanonical */ true);
  shovelerComponentUpdateField(
      entity2Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE,
      &entity2Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE],
      /* isCanonical */ true);

  // Nobody should see updates to entity 3.
  shovelerComponentUpdateField(
      entity3Component3,
      COMPONENT_TYPE_3_FIELD_DEPENDENCY,
      &entity3Component3->fieldValues[COMPONENT_TYPE_3_FIELD_DEPENDENCY],
      /* isCanonical */ true);

  ASSERT_THAT(
      ReceiveClientOps(client1),
      ElementsAre(
          IsUpdateComponentOp(testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE),
          IsUpdateComponentOp(
              testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE),
          IsUpdateComponentOp(
              testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE),
          IsUpdateComponentOp(
              testEntityId1, componentType2Id, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE),
          IsUpdateComponentOp(testEntityId2, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE),
          IsUpdateComponentOp(
              testEntityId2, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE),
          IsUpdateComponentOp(
              testEntityId2, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE)));
  ASSERT_THAT(
      ReceiveClientOps(client2),
      ElementsAre(
          IsUpdateComponentOp(testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE),
          IsUpdateComponentOp(
              testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE),
          IsUpdateComponentOp(
              testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE),
          IsUpdateComponentOp(
              testEntityId1, componentType2Id, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE)));
  ASSERT_THAT(ReceiveClientOps(client3), IsEmpty());
}

TEST_F(ShovelerViewSynchronizerTest, reassignAuthority) {
  void* clientHandle1 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  void* clientHandle2 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client1 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle1);
  auto* client2 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle2);
  SendAddEntityInterest(client1, testEntityId1);
  SendAddEntityInterest(client2, testEntityId1);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  auto clientId1 = clientConnectedCalls[0];
  auto clientId2 = clientConnectedCalls[1];

  // Flush received ops for all clients.
  ReceiveClientOps(client1);
  ReceiveClientOps(client2);

  // Delegate the two components to one client each.
  shovelerServerControllerDelegateComponent(server, testEntityId1, componentType1Id, clientId1);
  shovelerServerControllerDelegateComponent(server, testEntityId1, componentType2Id, clientId2);
  ASSERT_THAT(
      ReceiveClientOps(client1),
      ElementsAre(IsDelegateComponentOp(testEntityId1, componentType1Id)));
  ASSERT_THAT(
      ReceiveClientOps(client2),
      ElementsAre(IsDelegateComponentOp(testEntityId1, componentType2Id)));

  // Reassign authority over first component to second client.
  shovelerServerControllerDelegateComponent(server, testEntityId1, componentType1Id, clientId2);
  ASSERT_THAT(
      ReceiveClientOps(client1),
      ElementsAre(IsUndelegateComponentOp(testEntityId1, componentType1Id)));
  ASSERT_THAT(
      ReceiveClientOps(client2),
      ElementsAre(IsDelegateComponentOp(testEntityId1, componentType1Id)));
}

TEST_F(ShovelerViewSynchronizerTest, authoritativeUpdate) {
  const char* testComponentFieldValue = "the bird is the word";

  void* clientHandle1 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  void* clientHandle2 = shovelerInMemoryNetworkAdapterConnectClient(inMemoryNetworkAdapter);
  auto* client1 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle1);
  auto* client2 = shovelerInMemoryNetworkAdapterGetClient(inMemoryNetworkAdapter, clientHandle2);

  SendAddEntityInterest(client1, testEntityId1);
  SendAddEntityInterest(client2, testEntityId1);
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  auto clientId1 = clientConnectedCalls[0];

  shovelerServerControllerDelegateComponent(server, testEntityId1, componentType2Id, clientId1);

  // Flush received ops for all clients.
  ReceiveClientOps(client1);
  ReceiveClientOps(client2);

  // Send a component update from the authoritative client.
  ShovelerComponentFieldValue fieldValue;
  shovelerComponentFieldInitValue(&fieldValue, SHOVELER_COMPONENT_FIELD_TYPE_STRING);
  fieldValue.isSet = true;
  fieldValue.stringValue = const_cast<char*>(testComponentFieldValue); // not modified
  bool sent = SendUpdateComponent(
      client1,
      testEntityId1,
      componentType2Id,
      COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
      &fieldValue);
  ASSERT_TRUE(sent);

  // Assert that the other client receives it.
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(ReceiveClientOps(client1), IsEmpty());
  ASSERT_THAT(
      ReceiveClientOps(client2),
      ElementsAre(IsUpdateComponentOp(
          testEntityId1,
          componentType2Id,
          COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
          &fieldValue)));

  // Send a component update from the non-authoritative client.
  bool nonAuthoritativeSent = SendUpdateComponent(
      client2,
      testEntityId1,
      componentType2Id,
      COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
      &fieldValue);
  ASSERT_TRUE(nonAuthoritativeSent);

  // Assert that the other client doesn't receive it.
  shovelerViewSynchronizerUpdate(viewSynchronizer);
  ASSERT_THAT(ReceiveClientOps(client1), IsEmpty());
  ASSERT_THAT(ReceiveClientOps(client2), IsEmpty());
}
