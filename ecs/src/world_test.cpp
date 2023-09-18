#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ecs/src/component_field_value_wrapper.h"
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

void onAddEntity(ShovelerWorld* world, ShovelerWorldEntity* entity, void* userData);
void onRemoveEntity(ShovelerWorld* world, long long int entityId, void* userData);
void onAddComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* userData);
void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* userData);
void onActivateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* userData);
void onDeactivateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* userData);
void onDelegateComponent(
    ShovelerWorld* world, ShovelerWorldEntity* entity, const char* componentTypeId, void* userData);
void onUndelegateComponent(
    ShovelerWorld* world, ShovelerWorldEntity* entity, const char* componentTypeId, void* userData);
void onRemoveComponent(
    ShovelerWorld* world, ShovelerWorldEntity* entity, const char* componentTypeId, void* userData);
void onAddDependency(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    void* userData);
void onRemoveDependency(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    void* userData);

static void* activateComponent(ShovelerComponent* component, void* userData);
static void deactivateComponent(ShovelerComponent* component, void* userData);

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
    componentSystem2->requiresAuthority = true;
    componentSystem1->fieldOptions[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE].liveUpdateField =
        shovelerComponentSystemLiveUpdateFieldNoop;
    componentSystem1->activateComponent = activateComponent;
    componentSystem2->activateComponent = activateComponent;
    componentSystem3->activateComponent = activateComponent;
    componentSystem1->deactivateComponent = deactivateComponent;
    componentSystem2->deactivateComponent = deactivateComponent;
    componentSystem3->deactivateComponent = deactivateComponent;
    componentSystem1->callbackUserData = this;
    componentSystem2->callbackUserData = this;
    componentSystem3->callbackUserData = this;

    callbacks = shovelerWorldCallbacks();
    callbacks.onAddEntity = onAddEntity;
    callbacks.onRemoveEntity = onRemoveEntity;
    callbacks.onAddComponent = onAddComponent;
    callbacks.onUpdateComponent = onUpdateComponent;
    callbacks.onActivateComponent = onActivateComponent;
    callbacks.onDeactivateComponent = onDeactivateComponent;
    callbacks.onDelegateComponent = onDelegateComponent;
    callbacks.onUndelegateComponent = onUndelegateComponent;
    callbacks.onRemoveComponent = onRemoveComponent;
    callbacks.onAddDependency = onAddDependency;
    callbacks.onRemoveDependency = onRemoveDependency;
    callbacks.userData = this;

    world = shovelerWorldCreate(schema, system, &callbacks);
  }

  virtual void TearDown() {
    shovelerLogTrace("Tearing down test case.");
    shovelerWorldFree(world);
    shovelerSystemFree(system);
    shovelerSchemaFree(schema);
  }

  ShovelerSchema* schema;
  ShovelerSystem* system;
  ShovelerWorldCallbacks callbacks;
  ShovelerWorld* world;

  struct OnAddEntityCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;

    bool operator==(const OnAddEntityCall& other) const {
      return world == other.world && entity == other.entity;
    }
  };
  std::vector<OnAddEntityCall> onAddEntityCalls;

  struct OnRemoveEntityCall {
    ShovelerWorld* world;
    long long int entityId;

    bool operator==(const OnRemoveEntityCall& other) const {
      return world == other.world && entityId == other.entityId;
    }
  };
  std::vector<OnRemoveEntityCall> onRemoveEntityCalls;

  struct OnAddComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    ShovelerComponent* component;

    bool operator==(const OnAddComponentCall& other) const {
      return world == other.world && entity == other.entity && component == other.component;
    }
  };
  std::vector<OnAddComponentCall> onAddComponentCalls;

  struct OnUpdateComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    ShovelerComponent* component;
    int fieldId;
    const ShovelerComponentField* field;
    ShovelerComponentFieldValueWrapper value;
    bool isAuthoritative;
  };
  std::vector<OnUpdateComponentCall> onUpdateComponentCalls;

  struct OnActivateComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    ShovelerComponent* component;

    bool operator==(const OnActivateComponentCall& other) const {
      return world == other.world && entity == other.entity && component == other.component;
    }
  };
  std::vector<OnActivateComponentCall> onActivateComponentCalls;

  struct OnDeactivateComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    ShovelerComponent* component;

    bool operator==(const OnDeactivateComponentCall& other) const {
      return world == other.world && entity == other.entity && component == other.component;
    }
  };
  std::vector<OnDeactivateComponentCall> onDeactivateComponentCalls;

  struct OnDelegateComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    const char* componentTypeId;

    bool operator==(const OnDelegateComponentCall& other) const {
      return world == other.world && entity == other.entity &&
          componentTypeId == other.componentTypeId;
    }
  };
  std::vector<OnDelegateComponentCall> onDelegateComponentCalls;

  struct OnUndelegateComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    const char* componentTypeId;

    bool operator==(const OnUndelegateComponentCall& other) const {
      return world == other.world && entity == other.entity &&
          componentTypeId == other.componentTypeId;
    }
  };
  std::vector<OnUndelegateComponentCall> onUndelegateComponentCalls;

  struct OnRemoveComponentCall {
    ShovelerWorld* world;
    ShovelerWorldEntity* entity;
    const char* componentTypeId;

    bool operator==(const OnRemoveComponentCall& other) const {
      return world == other.world && entity == other.entity &&
          componentTypeId == other.componentTypeId;
    }
  };
  std::vector<OnRemoveComponentCall> onRemoveComponentCalls;

  struct OnAddDependencyCall {
    ShovelerWorld* world;
    ShovelerEntityComponentId dependencySource;
    ShovelerEntityComponentId dependencyTarget;

    bool operator==(const OnAddDependencyCall& other) const {
      return world == other.world && dependencySource.entityId == other.dependencySource.entityId &&
          dependencySource.componentTypeId == other.dependencySource.componentTypeId &&
          dependencyTarget.entityId == other.dependencyTarget.entityId &&
          dependencyTarget.componentTypeId == other.dependencyTarget.componentTypeId;
    }
  };
  std::vector<OnAddDependencyCall> onAddDependencyCalls;

  struct OnRemoveDependencyCall {
    ShovelerWorld* world;
    ShovelerEntityComponentId dependencySource;
    ShovelerEntityComponentId dependencyTarget;

    bool operator==(const OnRemoveDependencyCall& other) const {
      return world == other.world && dependencySource.entityId == other.dependencySource.entityId &&
          dependencySource.componentTypeId == other.dependencySource.componentTypeId &&
          dependencyTarget.entityId == other.dependencyTarget.entityId &&
          dependencyTarget.componentTypeId == other.dependencyTarget.componentTypeId;
    }
  };
  std::vector<OnRemoveDependencyCall> onRemoveDependencyCalls;

  std::vector<ShovelerComponent*> activateComponentCalls;
  std::vector<ShovelerComponent*> deactivateComponentCalls;
};

MATCHER_P7(
    IsOnUpdateComponentIntValueCall,
    world,
    entity,
    component,
    fieldId,
    field,
    intValue,
    isAuthoritative,
    "") {
  const ShovelerWorldTest::OnUpdateComponentCall& call = arg;
  return call.world == world && call.entity && entity && call.component == component &&
      call.fieldId == fieldId && call.field == field &&
      call.value->type == SHOVELER_COMPONENT_FIELD_TYPE_INT && call.value->isSet &&
      call.value->intValue == intValue && call.isAuthoritative == isAuthoritative;
}

TEST_F(ShovelerWorldTest, addRemoveEntity) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerWorldEntity* entity2 = shovelerWorldAddEntity(world, entityId2);
  ASSERT_THAT(
      onAddEntityCalls,
      ElementsAre(OnAddEntityCall{world, entity1}, OnAddEntityCall{world, entity2}));
  ASSERT_NE(entity1, entity2);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId1), entity1);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId2), entity2);
  ASSERT_EQ(shovelerWorldGetEntity(world, 1337), nullptr);

  bool removed = shovelerWorldRemoveEntity(world, entityId1);
  ASSERT_TRUE(removed);
  ASSERT_THAT(onRemoveEntityCalls, ElementsAre(OnRemoveEntityCall{world, entityId1}));
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId1), nullptr);
  ASSERT_EQ(shovelerWorldGetEntity(world, entityId2), entity2);
  ASSERT_EQ(shovelerWorldGetEntity(world, 1337), nullptr);

  onRemoveEntityCalls.clear();
  bool removedAgain = shovelerWorldRemoveEntity(world, entityId1);
  ASSERT_FALSE(removedAgain);
  ASSERT_THAT(onRemoveEntityCalls, IsEmpty());
}

TEST_F(ShovelerWorldTest, addRemoveComponent) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerWorldEntity* entity2 = shovelerWorldAddEntity(world, entityId2);
  ShovelerComponent* component1 =
      shovelerWorldEntityAddComponent(entity1, componentType1Id, /* status */ NULL);
  ASSERT_NE(component1, nullptr);
  ASSERT_EQ(component1->entityId, entityId1);
  ASSERT_THAT(onAddComponentCalls, ElementsAre(OnAddComponentCall{world, entity1, component1}));
  ShovelerComponent* addedAgain =
      shovelerWorldEntityAddComponent(entity1, componentType1Id, /* status */ NULL);
  ASSERT_EQ(addedAgain, nullptr) << "cannot add the same component twice";

  onAddComponentCalls.clear();
  ShovelerComponent* component2 =
      shovelerWorldEntityAddComponent(entity1, componentType2Id, /* status */ NULL);
  ASSERT_NE(component2, nullptr);
  ASSERT_NE(component1, component2);
  ASSERT_EQ(component2->entityId, entityId1);
  ASSERT_THAT(onAddComponentCalls, ElementsAre(OnAddComponentCall{world, entity1, component2}));
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType1Id), component1);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType2Id), component2);

  onAddComponentCalls.clear();
  ShovelerComponent* component3 =
      shovelerWorldEntityAddComponent(entity2, componentType1Id, /* status */ NULL);
  ASSERT_NE(component3, nullptr);
  ASSERT_NE(component1, component3);
  ASSERT_EQ(component3->entityId, entityId2);
  ASSERT_THAT(onAddComponentCalls, ElementsAre(OnAddComponentCall{world, entity2, component3}));
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity2, componentType1Id), component3);

  bool removed = shovelerWorldEntityRemoveComponent(entity1, componentType1Id);
  ASSERT_TRUE(removed);
  ASSERT_THAT(
      onRemoveComponentCalls, ElementsAre(OnRemoveComponentCall{world, entity1, componentType1Id}));
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType1Id), nullptr);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity1, componentType2Id), component2);
  ASSERT_EQ(shovelerWorldEntityGetComponent(entity2, componentType1Id), component3);
  bool removedAgain = shovelerWorldEntityRemoveComponent(entity1, componentType1Id);
  ASSERT_FALSE(removedAgain);
}

TEST_F(ShovelerWorldTest, updateAuthoritativeComponent) {
  const int newFieldValue = 27;

  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  shovelerWorldEntityDelegateComponent(entity1, componentType1Id);
  ShovelerComponent* component1 =
      shovelerWorldEntityAddComponent(entity1, componentType1Id, /* status */ NULL);

  bool updated =
      shovelerComponentUpdateFieldInt(component1, COMPONENT_TYPE_1_FIELD_PRIMITIVE, newFieldValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(
      onUpdateComponentCalls,
      ElementsAre(IsOnUpdateComponentIntValueCall(
          world,
          entity1,
          component1,
          COMPONENT_TYPE_1_FIELD_PRIMITIVE,
          &component1->type->fields[COMPONENT_TYPE_1_FIELD_PRIMITIVE],
          newFieldValue,
          /* isAuthoritative */ true)));
}

TEST_F(ShovelerWorldTest, delegateUndelegate) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerComponent* component1 =
      shovelerWorldEntityAddComponent(entity1, componentType1Id, /* status */ NULL);
  ShovelerComponent* component2 =
      shovelerWorldEntityAddComponent(entity1, componentType2Id, /* status */ NULL);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId1);

  // Try to activate component 2, which should fail because it requires authority but isn't
  // delegated.
  bool activated = shovelerComponentActivate(component2);
  ASSERT_FALSE(activated);

  shovelerWorldEntityDelegateComponent(entity1, componentType2Id);
  ASSERT_THAT(
      onDelegateComponentCalls,
      ElementsAre(OnDelegateComponentCall{world, entity1, componentType2Id}));

  activated = shovelerComponentActivate(component2);
  ASSERT_TRUE(activated);
  ASSERT_THAT(activateComponentCalls, ElementsAre(component2, component1));
  ASSERT_THAT(
      onActivateComponentCalls,
      ElementsAre(
          OnActivateComponentCall{world, entity1, component2},
          OnActivateComponentCall{world, entity1, component1}));

  // Undelegate component 2, which should both deactivate it and its reverse dependency.
  shovelerWorldEntityUndelegateComponent(entity1, componentType2Id);
  ASSERT_THAT(
      onUndelegateComponentCalls,
      ElementsAre(OnUndelegateComponentCall{world, entity1, componentType2Id}));
  ASSERT_THAT(deactivateComponentCalls, ElementsAre(component1, component2));
  ASSERT_THAT(
      onDeactivateComponentCalls,
      ElementsAre(
          OnDeactivateComponentCall{world, entity1, component1},
          OnDeactivateComponentCall{world, entity1, component2}));
}

TEST_F(ShovelerWorldTest, addRemoveDependency) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerComponent* component1 =
      shovelerWorldEntityAddComponent(entity1, componentType1Id, /* status */ NULL);
  ShovelerComponent* component2 =
      shovelerWorldEntityAddComponent(entity1, componentType2Id, /* status */ NULL);
  ASSERT_THAT(onAddDependencyCalls, IsEmpty()); // dependency fields are optional

  shovelerWorldEntityDelegateComponent(entity1, componentType2Id);
  ASSERT_THAT(
      onDelegateComponentCalls,
      ElementsAre(OnDelegateComponentCall{world, entity1, componentType2Id}));

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId1);
  ASSERT_THAT(
      onAddDependencyCalls,
      ElementsAre(OnAddDependencyCall{
          world,
          shovelerEntityComponentId(entityId1, componentType1Id),
          shovelerEntityComponentId(entityId1, componentType2Id)}));

  shovelerComponentActivate(component2);
  ASSERT_THAT(activateComponentCalls, ElementsAre(component2, component1));
  ASSERT_THAT(
      onActivateComponentCalls,
      ElementsAre(
          OnActivateComponentCall{world, entity1, component2},
          OnActivateComponentCall{world, entity1, component1}));

  onAddDependencyCalls.clear();
  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);
  ASSERT_THAT(
      onRemoveDependencyCalls,
      ElementsAre(OnRemoveDependencyCall{
          world,
          shovelerEntityComponentId(entityId1, componentType1Id),
          shovelerEntityComponentId(entityId1, componentType2Id)}));
  ASSERT_THAT(
      onAddDependencyCalls,
      ElementsAre(OnAddDependencyCall{
          world,
          shovelerEntityComponentId(entityId1, componentType1Id),
          shovelerEntityComponentId(entityId2, componentType2Id)}));
  ASSERT_THAT(deactivateComponentCalls, ElementsAre(component1));
  ASSERT_THAT(
      onDeactivateComponentCalls,
      ElementsAre(OnDeactivateComponentCall{world, entity1, component1}));
}

TEST_F(ShovelerWorldTest, addRemoveDoubleDependency) {
  ShovelerWorldEntity* entity1 = shovelerWorldAddEntity(world, entityId1);
  ShovelerComponent* component1 =
      shovelerWorldEntityAddComponent(entity1, componentType1Id, /* status */ NULL);
  ShovelerComponent* component2 =
      shovelerWorldEntityAddComponent(entity1, componentType2Id, /* status */ NULL);
  shovelerWorldEntityDelegateComponent(entity1, componentType2Id);

  // Set two dependencies from component1 to component2.
  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId1);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId1);
  ASSERT_THAT(
      onAddDependencyCalls,
      ElementsAre(
          OnAddDependencyCall{
              world,
              shovelerEntityComponentId(entityId1, componentType1Id),
              shovelerEntityComponentId(entityId1, componentType2Id)},
          OnAddDependencyCall{
              world,
              shovelerEntityComponentId(entityId1, componentType1Id),
              shovelerEntityComponentId(entityId1, componentType2Id)}));

  // Activate component2, make sure that component1 is also activated.
  shovelerComponentActivate(component2);
  ASSERT_THAT(activateComponentCalls, ElementsAre(component2, component1));
  ASSERT_THAT(
      onActivateComponentCalls,
      ElementsAre(
          OnActivateComponentCall{world, entity1, component2},
          OnActivateComponentCall{world, entity1, component1}));

  // Remove the dependency that can be live updated.
  onRemoveDependencyCalls.clear();
  shovelerComponentClearField(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, /* isAuthoritative */ true);
  ASSERT_THAT(
      onRemoveDependencyCalls,
      ElementsAre(OnRemoveDependencyCall{
          world,
          shovelerEntityComponentId(entityId1, componentType1Id),
          shovelerEntityComponentId(entityId1, componentType2Id)}));
  ASSERT_THAT(deactivateComponentCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());

  // If we deactivate component2, component1 should still also deactivate.
  shovelerComponentDeactivate(component2);
  ASSERT_THAT(deactivateComponentCalls, ElementsAre(component1, component2));
  ASSERT_THAT(
      onDeactivateComponentCalls,
      ElementsAre(
          OnDeactivateComponentCall{world, entity1, component1},
          OnDeactivateComponentCall{world, entity1, component2}));
}

void onAddEntity(ShovelerWorld* world, ShovelerWorldEntity* entity, void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onAddEntityCalls.emplace_back(ShovelerWorldTest::OnAddEntityCall{world, entity});
}

void onRemoveEntity(ShovelerWorld* world, long long int entityId, void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onRemoveEntityCalls.emplace_back(ShovelerWorldTest::OnRemoveEntityCall{world, entityId});
}

void onAddComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onAddComponentCalls.emplace_back(
      ShovelerWorldTest::OnAddComponentCall{world, entity, component});
}

void onUpdateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isAuthoritative,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onUpdateComponentCalls.push_back(ShovelerWorldTest::OnUpdateComponentCall{
      world, entity, component, fieldId, field, value, isAuthoritative});
}
void onActivateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onActivateComponentCalls.emplace_back(
      ShovelerWorldTest::OnActivateComponentCall{world, entity, component});
}

void onDeactivateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    ShovelerComponent* component,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onDeactivateComponentCalls.emplace_back(
      ShovelerWorldTest::OnDeactivateComponentCall{world, entity, component});
}

void onDelegateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    const char* componentTypeId,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onDelegateComponentCalls.emplace_back(
      ShovelerWorldTest::OnDelegateComponentCall{world, entity, componentTypeId});
}

void onUndelegateComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    const char* componentTypeId,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onUndelegateComponentCalls.emplace_back(
      ShovelerWorldTest::OnUndelegateComponentCall{world, entity, componentTypeId});
}

void onRemoveComponent(
    ShovelerWorld* world,
    ShovelerWorldEntity* entity,
    const char* componentTypeId,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onRemoveComponentCalls.emplace_back(
      ShovelerWorldTest::OnRemoveComponentCall{world, entity, componentTypeId});
}

void onAddDependency(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onAddDependencyCalls.emplace_back(
      ShovelerWorldTest::OnAddDependencyCall{world, *dependencySource, *dependencyTarget});
}

void onRemoveDependency(
    ShovelerWorld* world,
    const ShovelerEntityComponentId* dependencySource,
    const ShovelerEntityComponentId* dependencyTarget,
    void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->onRemoveDependencyCalls.emplace_back(
      ShovelerWorldTest::OnRemoveDependencyCall{world, *dependencySource, *dependencyTarget});
}

static void* activateComponent(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->activateComponentCalls.emplace_back(component);
  return test;
}

static void deactivateComponent(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerWorldTest*>(testPointer);
  test->deactivateComponentCalls.emplace_back(component);
}
