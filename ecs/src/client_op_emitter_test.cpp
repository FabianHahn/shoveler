extern "C" {
#include <shoveler/client_op_emitter.h>
#include <shoveler/component.h>
#include <shoveler/schema.h>
#include <shoveler/system.h>
#include <shoveler/world.h>

#include "test_component_types.h"
}

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "client_op_wrapper.h"

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Matcher;
using ::testing::Not;
using ::testing::UnorderedElementsAre;

namespace {

const int64_t testClientId1 = 1;
const int64_t testClientId2 = 2;
const long long int testEntityId1 = 1;
const long long int testEntityId2 = 2;
const long long int testEntityId3 = 3;

void prepareEntityInterest(long long int entityId, GArray* clientIdArray, void* userData);
void prepareClientAuthority(
    int64_t clientId, long long int entityId, GArray* componentTypeIdArray, void* userData);
void prepareClientActivations(
    int64_t clientId,
    long long int entityId,
    GArray* componentTypeIdArray,
    void* viewSynchronizerPointer);
void onEmitOp(
    ShovelerClientOpEmitter* clientOpEmitter,
    const int64_t* clientIds,
    int numClients,
    const ShovelerClientOp* clientOp,
    void* userData);

class ShovelerClientOpEmitterTest : public ::testing::Test {
public:
  virtual void SetUp() {
    schema = shovelerSchemaCreate();
    ShovelerComponentType* componentType1 = shovelerCreateTestComponentType1();
    ShovelerComponentType* componentType2 = shovelerCreateTestComponentType2();
    ShovelerComponentType* componentType3 = shovelerCreateTestComponentType3();
    shovelerSchemaAddComponentType(schema, componentType1);
    shovelerSchemaAddComponentType(schema, componentType2);
    shovelerSchemaAddComponentType(schema, componentType3);

    system = shovelerSystemCreate();
    worldCallbacks = shovelerWorldCallbacks();
    world = shovelerWorldCreate(schema, system, &worldCallbacks);

    entity1 = shovelerWorldAddEntity(world, testEntityId1);
    entity1Component1 = shovelerWorldEntityAddComponent(entity1, componentType1Id);
    entity1Component2 = shovelerWorldEntityAddComponent(entity1, componentType2Id);
    entity2 = shovelerWorldAddEntity(world, testEntityId2);
    entity2Component1 = shovelerWorldEntityAddComponent(entity2, componentType1Id);
    entity3 = shovelerWorldAddEntity(world, testEntityId3);
    entity3Component3 = shovelerWorldEntityAddComponent(entity3, componentType3Id);

    clientOpEmitterAdapter.prepareEntityInterest = prepareEntityInterest;
    clientOpEmitterAdapter.prepareClientAuthority = prepareClientAuthority;
    clientOpEmitterAdapter.prepareClientActivations = prepareClientActivations;
    clientOpEmitterAdapter.onEmitOp = onEmitOp;
    clientOpEmitterAdapter.userData = this;
    clientOpEmitter = shovelerClientOpEmitterCreate(&clientOpEmitterAdapter);
  }

  virtual void TearDown() {
    shovelerClientOpEmitterFree(clientOpEmitter);
    shovelerWorldFree(world);
    shovelerSystemFree(system);
    shovelerSchemaFree(schema);
  }

  ShovelerSchema* schema;
  ShovelerSystem* system;
  ShovelerWorldCallbacks worldCallbacks;
  ShovelerWorld* world;

  ShovelerWorldEntity* entity1;
  ShovelerComponent* entity1Component1;
  ShovelerComponent* entity1Component2;
  ShovelerWorldEntity* entity2;
  ShovelerComponent* entity2Component1;
  ShovelerWorldEntity* entity3;
  ShovelerComponent* entity3Component3;

  ShovelerClientOpEmitterAdapter clientOpEmitterAdapter;
  ShovelerClientOpEmitter* clientOpEmitter;

  struct EmittedOp {
    std::vector<int64_t> clientIds;
    ShovelerClientOpWrapper clientOp;
  };
  std::vector<EmittedOp> emittedOps;
};

void prepareEntityInterest(long long int entityId, GArray* clientIdArray, void* testPointer) {
  if (entityId == testEntityId1) {
    g_array_set_size(clientIdArray, 2);
    g_array_index(clientIdArray, int64_t, 0) = testClientId1;
    g_array_index(clientIdArray, int64_t, 1) = testClientId2;
  } else if (entityId == testEntityId2) {
    g_array_set_size(clientIdArray, 1);
    g_array_index(clientIdArray, int64_t, 0) = testClientId1;
  } else {
    g_array_set_size(clientIdArray, 0);
  }
}

void prepareClientAuthority(
    int64_t clientId, long long int entityId, GArray* componentTypeIdArray, void* userData) {
  if (entityId == testEntityId1) {
    g_array_set_size(componentTypeIdArray, 1);
    g_array_index(componentTypeIdArray, const char*, 0) = componentType3Id;
  } else {
    g_array_set_size(componentTypeIdArray, 0);
  }
}

void prepareClientActivations(
    int64_t clientId,
    long long int entityId,
    GArray* componentTypeIdArray,
    void* viewSynchronizerPointer) {
  if (entityId == testEntityId1) {
    g_array_set_size(componentTypeIdArray, 1);
    g_array_index(componentTypeIdArray, const char*, 0) = componentType1Id;
  } else {
    g_array_set_size(componentTypeIdArray, 0);
  }
}

void onEmitOp(
    ShovelerClientOpEmitter* clientOpEmitter,
    const int64_t* clientIds,
    int numClients,
    const ShovelerClientOp* clientOp,
    void* testPointer) {
  auto* test = static_cast<ShovelerClientOpEmitterTest*>(testPointer);
  test->emittedOps.push_back(ShovelerClientOpEmitterTest::EmittedOp{
      std::vector<int64_t>(clientIds, clientIds + numClients), ShovelerClientOpWrapper{clientOp}});
}

std::ostream& operator<<(
    std::ostream& output, const ShovelerClientOpEmitterTest::EmittedOp& emittedOp) {
  output << "EmittedOp(clientIds = [";

  bool first = true;
  for (auto clientId : emittedOp.clientIds) {
    if (first) {
      first = false;
    } else {
      output << ",";
    }

    output << clientId;
  }

  output << "], op = " << emittedOp.clientOp << ")";
  return output;
}

auto IsEmittedOp(
    Matcher<std::vector<int64_t>> clientIds, Matcher<ShovelerClientOpWrapper> clientOp) {
  return AllOf(
      Field(&ShovelerClientOpEmitterTest::EmittedOp::clientIds, clientIds),
      Field(&ShovelerClientOpEmitterTest::EmittedOp::clientOp, clientOp));
}

} // namespace

TEST_F(ShovelerClientOpEmitterTest, checkout) {
  shovelerClientOpEmitterCheckoutEntity(clientOpEmitter, entity1, testClientId1);
  ASSERT_THAT(
      emittedOps,
      UnorderedElementsAre(
          IsEmittedOp(ElementsAre(testClientId1), IsAddEntityOp(testEntityId1)),
          IsEmittedOp(
              ElementsAre(testClientId1), IsAddComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              ElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE)),
          IsEmittedOp(
              ElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE)),
          IsEmittedOp(
              ElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE)),
          IsEmittedOp(
              ElementsAre(testClientId1), IsActivateComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              ElementsAre(testClientId1), IsAddComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              ElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId1, componentType2Id, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE)),
          IsEmittedOp(
              ElementsAre(testClientId1), IsDelegateComponentOp(testEntityId1, componentType3Id))));
}

TEST_F(ShovelerClientOpEmitterTest, uncheckout) {
  shovelerClientOpEmitterCheckoutEntity(clientOpEmitter, entity1, testClientId1);

  emittedOps.clear();
  shovelerClientOpEmitterUncheckoutEntity(clientOpEmitter, testEntityId1, testClientId1);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(IsEmittedOp(ElementsAre(testClientId1), IsRemoveEntityOp(testEntityId1))));
}

TEST_F(ShovelerClientOpEmitterTest, addEntity) {
  shovelerClientOpEmitterAddEntity(clientOpEmitter, entity1);
  shovelerClientOpEmitterAddEntity(clientOpEmitter, entity2);
  shovelerClientOpEmitterAddEntity(clientOpEmitter, entity3);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2), IsAddEntityOp(testEntityId1)),
          IsEmittedOp(UnorderedElementsAre(testClientId1), IsAddEntityOp(testEntityId2))));
}

TEST_F(ShovelerClientOpEmitterTest, removeEntity) {
  shovelerClientOpEmitterRemoveEntity(clientOpEmitter, testEntityId1);
  shovelerClientOpEmitterRemoveEntity(clientOpEmitter, testEntityId2);
  shovelerClientOpEmitterRemoveEntity(clientOpEmitter, testEntityId3);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2), IsRemoveEntityOp(testEntityId1)),
          IsEmittedOp(UnorderedElementsAre(testClientId1), IsRemoveEntityOp(testEntityId2))));
}

TEST_F(ShovelerClientOpEmitterTest, addComponent) {
  shovelerClientOpEmitterAddComponent(clientOpEmitter, entity1Component1);
  shovelerClientOpEmitterAddComponent(clientOpEmitter, entity1Component2);
  shovelerClientOpEmitterAddComponent(clientOpEmitter, entity2Component1);
  shovelerClientOpEmitterAddComponent(clientOpEmitter, entity3Component3);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsAddComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsAddComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsAddComponentOp(testEntityId2, componentType1Id))));
}

TEST_F(ShovelerClientOpEmitterTest, updateComponent) {
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity1Component1,
      COMPONENT_TYPE_1_FIELD_PRIMITIVE,
      &entity1Component1->fieldValues[COMPONENT_TYPE_1_FIELD_PRIMITIVE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity1Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
      &entity1Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity1Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE,
      &entity1Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity1Component2,
      COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
      &entity1Component2->fieldValues[COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity2Component1,
      COMPONENT_TYPE_1_FIELD_PRIMITIVE,
      &entity2Component1->fieldValues[COMPONENT_TYPE_1_FIELD_PRIMITIVE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity2Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
      &entity2Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity2Component1,
      COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE,
      &entity2Component1->fieldValues[COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE]);
  shovelerClientOpEmitterUpdateComponent(
      clientOpEmitter,
      entity3Component3,
      COMPONENT_TYPE_3_FIELD_DEPENDENCY,
      &entity3Component3->fieldValues[COMPONENT_TYPE_3_FIELD_DEPENDENCY]);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsUpdateComponentOp(
                  testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsUpdateComponentOp(
                  testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsUpdateComponentOp(
                  testEntityId1, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsUpdateComponentOp(
                  testEntityId1, componentType2Id, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId2, componentType1Id, COMPONENT_TYPE_1_FIELD_PRIMITIVE)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId2, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsUpdateComponentOp(
                  testEntityId2, componentType1Id, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE))));
}

TEST_F(ShovelerClientOpEmitterTest, delegateComponent) {
  shovelerClientOpEmitterDelegateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId1);
  shovelerClientOpEmitterDelegateComponent(
      clientOpEmitter, testEntityId1, componentType2Id, testClientId1);
  shovelerClientOpEmitterDelegateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId2);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsDelegateComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsDelegateComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId2),
              IsDelegateComponentOp(testEntityId1, componentType1Id))));
}

TEST_F(ShovelerClientOpEmitterTest, undelegateComponent) {
  shovelerClientOpEmitterUndelegateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId1);
  shovelerClientOpEmitterUndelegateComponent(
      clientOpEmitter, testEntityId1, componentType2Id, testClientId1);
  shovelerClientOpEmitterUndelegateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId2);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsUndelegateComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsUndelegateComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId2),
              IsUndelegateComponentOp(testEntityId1, componentType1Id))));
}

TEST_F(ShovelerClientOpEmitterTest, activateComponent) {
  shovelerClientOpEmitterActivateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId1);
  shovelerClientOpEmitterActivateComponent(
      clientOpEmitter, testEntityId1, componentType2Id, testClientId1);
  shovelerClientOpEmitterActivateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId2);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsActivateComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsActivateComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId2),
              IsActivateComponentOp(testEntityId1, componentType1Id))));
}

TEST_F(ShovelerClientOpEmitterTest, deactivateComponent) {
  shovelerClientOpEmitterDeactivateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId1);
  shovelerClientOpEmitterDeactivateComponent(
      clientOpEmitter, testEntityId1, componentType2Id, testClientId1);
  shovelerClientOpEmitterDeactivateComponent(
      clientOpEmitter, testEntityId1, componentType1Id, testClientId2);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsDeactivateComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsDeactivateComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId2),
              IsDeactivateComponentOp(testEntityId1, componentType1Id))));
}

TEST_F(ShovelerClientOpEmitterTest, removeComponent) {
  shovelerClientOpEmitterRemoveComponent(clientOpEmitter, testEntityId1, componentType1Id);
  shovelerClientOpEmitterRemoveComponent(clientOpEmitter, testEntityId1, componentType2Id);
  shovelerClientOpEmitterRemoveComponent(clientOpEmitter, testEntityId2, componentType1Id);
  shovelerClientOpEmitterRemoveComponent(clientOpEmitter, testEntityId3, componentType3Id);
  ASSERT_THAT(
      emittedOps,
      ElementsAre(
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsRemoveComponentOp(testEntityId1, componentType1Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1, testClientId2),
              IsRemoveComponentOp(testEntityId1, componentType2Id)),
          IsEmittedOp(
              UnorderedElementsAre(testClientId1),
              IsRemoveComponentOp(testEntityId2, componentType1Id))));
}