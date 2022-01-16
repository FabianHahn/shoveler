#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/component.h"
#include "shoveler/component_system.h"
#include "shoveler/component_type.h"
#include "shoveler/entity_component_id.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"
#include "shoveler/world.h"
#include "test_component_types.h"
}

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;

const long long int entityId1 = 1;
const long long int entityId2 = 2;

struct DependencyCallbackCall {
  ShovelerWorld* world;
  ShovelerEntityComponentId dependencySource;
  ShovelerEntityComponentId dependencyTarget;
  bool added;
};

static void updateAuthoritativeComponent(
    ShovelerWorld* world,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* userData);
static void dependencyCallback(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    bool added,
    void* userData);

static void* activateComponent(ShovelerComponent* component, void* userData);
static void deactivateComponent(ShovelerComponent* component, void* userData);

MATCHER_P4(
    IsAddedDependency,
    sourceEntityId,
    sourceComponentTypeId,
    targetEntityId,
    targetComponentTypeId,
    "") {
  const DependencyCallbackCall& call = arg;
  return call.dependencySource.entityId == sourceEntityId &&
      call.dependencySource.componentTypeId == sourceComponentTypeId &&
      call.dependencyTarget.entityId == targetEntityId &&
      call.dependencyTarget.componentTypeId == targetComponentTypeId && call.added;
}

MATCHER_P4(
    IsRemovedDependency,
    sourceEntityId,
    sourceComponentTypeId,
    targetEntityId,
    targetComponentTypeId,
    "") {
  const DependencyCallbackCall& call = arg;
  return call.dependencySource.entityId == sourceEntityId &&
      call.dependencySource.componentTypeId == sourceComponentTypeId &&
      call.dependencyTarget.entityId == targetEntityId &&
      call.dependencyTarget.componentTypeId == targetComponentTypeId && !call.added;
}

class ShovelerWorldTest : public ::testing::Test {
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
    ShovelerComponentSystem* componentSystem1 =
        shovelerSystemForComponentType(system, componentType1);
    ShovelerComponentSystem* componentSystem2 =
        shovelerSystemForComponentType(system, componentType2);
    ShovelerComponentSystem* componentSystem3 =
        shovelerSystemForComponentType(system, componentType3);
    componentSystem1->activateComponent = activateComponent;
    componentSystem2->activateComponent = activateComponent;
    componentSystem3->activateComponent = activateComponent;
    componentSystem1->deactivateComponent = deactivateComponent;
    componentSystem2->deactivateComponent = deactivateComponent;
    componentSystem3->deactivateComponent = deactivateComponent;
    componentSystem1->callbackUserData = this;
    componentSystem2->callbackUserData = this;
    componentSystem3->callbackUserData = this;

    world = shovelerWorldCreate(schema, system, updateAuthoritativeComponent, this);
    shovelerWorldAddDependencyCallback(world, dependencyCallback, this);
  }

  virtual void TearDown() {
    shovelerLogTrace("Tearing down test case.");
    shovelerWorldFree(world);
    shovelerSystemFree(system);
    shovelerSchemaFree(schema);
  }

  ShovelerSchema* schema;
  ShovelerSystem* system;
  ShovelerWorld* world;

  struct UpdateAuthoritativeComponentCall {
    ShovelerWorld* world;
    ShovelerComponent* component;
    const ShovelerComponentField* field;
    const ShovelerComponentFieldValue* value;
  };
  std::vector<UpdateAuthoritativeComponentCall> updateAuthoritativeComponentCalls;

  std::vector<DependencyCallbackCall> dependencyCallbackCalls;
  std::vector<ShovelerComponent*> activateCalls;
  std::vector<ShovelerComponent*> deactivateCalls;
};

TEST_F(ShovelerWorldTest, addRemoveEntity) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerWorldEntity* entity2 = shovelerWorldAddEntity(world, entityId2);
  ASSERT_NE(entity1, entity2);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId1), entity1);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId2), entity2);
  ASSERT_EQ(shovelerWorldGetEntity(world, 1337), nullptr);

  bool removed = shovelerWorldRemoveEntity(world, entityId1);
  ASSERT_TRUE(removed);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId1), nullptr);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId2), entity2);
  ASSERT_EQ(shovelerWorldGetEntity(world, 1337), nullptr);
  bool removedAgain = shovelerWorldRemoveEntity(world, entityId1);
  ASSERT_FALSE(removedAgain);
}

TEST_F(ShovelerWorldTest, addRemoveComponent) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerWorldEntity* entity2 = shovelerWorldAddEntity(world, entityId2);
  ShovelerComponent* component1 = shovelerWorldEntityAddComponent(entity1, componentType1Id);
  ASSERT_NE(component1, nullptr);
  ASSERT_EQ(shovelerWorldEntityAddComponent(entity1, componentType1Id), nullptr)
      << "cannot add the same component twice";
  ShovelerComponent* component2 = shovelerWorldEntityAddComponent(entity1, componentType2Id);
  ASSERT_NE(component2, nullptr);
  ASSERT_NE(component1, component2);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType1Id), component1);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType2Id), component2);

  ShovelerComponent* component3 = shovelerWorldEntityAddComponent(entity2, componentType1Id);
  ASSERT_NE(component3, nullptr);
  ASSERT_NE(component1, component3);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity2, componentType1Id), component3);

  bool removed = shovelerWorldEntityRemoveComponent(entity1, componentType1Id);
  ASSERT_TRUE(removed);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType1Id), nullptr);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType2Id), component2);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity2, componentType1Id), component3);
  bool removedAgain = shovelerWorldEntityRemoveComponent(entity1, componentType1Id);
  ASSERT_FALSE(removedAgain);
}

TEST_F(ShovelerWorldTest, updateAuthoritativeComponent) {
  const int newConfigurationValue = 27;

  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  shovelerWorldEntityDelegateComponent(entity1, componentType1Id);
  ShovelerComponent* component1 = shovelerWorldEntityAddComponent(entity1, componentType1Id);

  bool updated = shovelerComponentUpdateFieldInt(
      component1, COMPONENT_TYPE_1_FIELD_PRIMITIVE, newConfigurationValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(updateAuthoritativeComponentCalls, SizeIs(1));
  ASSERT_EQ(updateAuthoritativeComponentCalls[0].world, world);
  ASSERT_EQ(updateAuthoritativeComponentCalls[0].component, component1);
  ASSERT_EQ(
      updateAuthoritativeComponentCalls[0].field,
      &component1->type->fields[COMPONENT_TYPE_1_FIELD_PRIMITIVE]);
}

TEST_F(ShovelerWorldTest, addRemoveDependency) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerComponent* component1 = shovelerWorldEntityAddComponent(entity1, componentType1Id);
  ShovelerComponent* component2 = shovelerWorldEntityAddComponent(entity1, componentType2Id);
  shovelerWorldEntityDelegateComponent(entity1, componentType2Id);

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId1);
  shovelerComponentActivate(component2);
  ASSERT_THAT(
      dependencyCallbackCalls,
      ElementsAre(IsAddedDependency(entityId1, componentType1Id, entityId1, componentType2Id)));
  dependencyCallbackCalls.clear();
  ASSERT_THAT(activateCalls, ElementsAre(component2, component1));

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);
  ASSERT_THAT(
      dependencyCallbackCalls,
      ElementsAre(
          IsRemovedDependency(entityId1, componentType1Id, entityId1, componentType2Id),
          IsAddedDependency(entityId1, componentType1Id, entityId2, componentType2Id)));
  ASSERT_THAT(deactivateCalls, ElementsAre(component1));
}

static void updateAuthoritativeComponent(
    ShovelerWorld* world,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    void* testPointer) {
  ShovelerWorldTest* test = (ShovelerWorldTest*) testPointer;
  test->updateAuthoritativeComponentCalls.emplace_back(
      ShovelerWorldTest::UpdateAuthoritativeComponentCall{world, component, field, value});
}

static void dependencyCallback(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    bool added,
    void* testPointer) {
  ShovelerWorldTest* test = (ShovelerWorldTest*) testPointer;
  test->dependencyCallbackCalls.emplace_back(
      DependencyCallbackCall{world, *dependencySource, *dependencyTarget, added});
}

static void* activateComponent(ShovelerComponent* component, void* testPointer) {
  ShovelerWorldTest* test = (ShovelerWorldTest*) testPointer;
  test->activateCalls.emplace_back(component);
  return test;
}

static void deactivateComponent(ShovelerComponent* component, void* testPointer) {
  ShovelerWorldTest* test = (ShovelerWorldTest*) testPointer;
  test->deactivateCalls.emplace_back(component);
}
