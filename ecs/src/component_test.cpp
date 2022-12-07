#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "shoveler/component.h"
#include "shoveler/component_type.h"
#include "shoveler/log.h"
#include "test_component_types.h"
}

#include "component_field_value_wrapper.h"

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;

const long long int entityId1 = 1;
const long long int entityId2 = 2;

// world adapter methods
static ShovelerComponent* getComponent(
    ShovelerComponent* component,
    long long int entityId,
    const char* componentTypeId,
    void* userData);
static void onUpdateComponentField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isCanonical,
    void* testPointer);
static void onActivateComponent(ShovelerComponent* component, void* userData);
static void onDeactivateComponent(ShovelerComponent* component, void* userData);
static void addDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* userData);
static bool removeDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* userData);
static void forEachReverseDependency(
    ShovelerComponent* component,
    ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction* callbackFunction,
    void* callbackUserData,
    void* adapterUserData);

// system adapter methods
static bool requiresAuthority(ShovelerComponent* component, void* userData);
static bool canLiveUpdateField(
    ShovelerComponent* component, int fieldId, const ShovelerComponentField* field, void* userData);
static bool canLiveUpdateDependencyField(
    ShovelerComponent* component, int fieldId, const ShovelerComponentField* field, void* userData);
static bool liveUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* userData);
static bool liveUpdateDependencyField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* userData);
static void* activateComponent(ShovelerComponent* component, void* userData);
static bool updateComponent(ShovelerComponent* component, double dt, void* userData);
static void deactivateComponent(ShovelerComponent* component, void* userData);

class ShovelerComponentTest : public ::testing::Test {
public:
  virtual void SetUp() {
    worldAdapter.getComponent = getComponent;
    worldAdapter.forEachReverseDependency = forEachReverseDependency;
    worldAdapter.addDependency = addDependency;
    worldAdapter.removeDependency = removeDependency;
    worldAdapter.onUpdateComponentField = onUpdateComponentField;
    worldAdapter.onActivateComponent = onActivateComponent;
    worldAdapter.onDeactivateComponent = onDeactivateComponent;

    worldAdapter.userData = this;

    systemAdapter.requiresAuthority = requiresAuthority;
    systemAdapter.canLiveUpdateField = canLiveUpdateField;
    systemAdapter.canLiveUpdateDependencyField = canLiveUpdateDependencyField;
    systemAdapter.liveUpdateField = liveUpdateField;
    systemAdapter.liveUpdateDependencyField = liveUpdateDependencyField;
    systemAdapter.activateComponent = activateComponent;
    systemAdapter.updateComponent = updateComponent;
    systemAdapter.deactivateComponent = deactivateComponent;
    systemAdapter.userData = this;

    componentType1 = shovelerCreateTestComponentType1();
    componentType2 = shovelerCreateTestComponentType2();
    componentType3 = shovelerCreateTestComponentType3();

    component1 = shovelerComponentCreate(&worldAdapter, &systemAdapter, entityId1, componentType1);
    component2 = shovelerComponentCreate(&worldAdapter, &systemAdapter, entityId2, componentType2);
    component3 = shovelerComponentCreate(&worldAdapter, &systemAdapter, entityId1, componentType3);
    ASSERT_TRUE(component1 != nullptr);
    ASSERT_TRUE(component2 != nullptr);
    ASSERT_TRUE(component3 != nullptr);
  }

  virtual void TearDown() {
    shovelerLogTrace("Tearing down test case.");
    shovelerComponentFree(component3);
    shovelerComponentFree(component2);
    shovelerComponentFree(component1);
    shovelerComponentTypeFree(componentType3);
    shovelerComponentTypeFree(componentType2);
    shovelerComponentTypeFree(componentType1);
  }

  ShovelerComponentWorldAdapter worldAdapter;
  ShovelerComponentSystemAdapter systemAdapter;

  ShovelerComponentType* componentType1;
  ShovelerComponentType* componentType2;
  ShovelerComponentType* componentType3;

  ShovelerComponent* component1;
  ShovelerComponent* component2;
  ShovelerComponent* component3;

  struct OnUpdateComponentFieldCall {
    ShovelerComponent* component;
    int fieldId;
    const ShovelerComponentField* field;
    ShovelerComponentFieldValueWrapper value;
    bool isCanonical;
  };
  std::vector<OnUpdateComponentFieldCall> onUpdateComponentFieldCalls;
  std::vector<ShovelerComponent*> onActivateComponentCalls;
  std::vector<ShovelerComponent*> onDeactivateComponentCalls;

  std::map<std::pair<long long int, std::string>, std::set<ShovelerComponent*>> reverseDependencies;

  bool propagateNextLiveUpdate = false;
  struct LiveUpdateCall {
    ShovelerComponent* component;
    int fieldId;
    const ShovelerComponentField* field;
    ShovelerComponentFieldValueWrapper value;
  };
  std::vector<LiveUpdateCall> liveUpdateCalls;

  bool propagateNextLiveUpdateDependency = false;
  struct LiveUpdateDependencyCall {
    ShovelerComponent* component;
    int fieldId;
    const ShovelerComponentField* field;
    ShovelerComponent* dependencyComponent;

    bool operator==(const LiveUpdateDependencyCall& other) const {
      return component == other.component && fieldId == other.fieldId && field == other.field &&
          dependencyComponent == other.dependencyComponent;
    }
  };
  std::vector<LiveUpdateDependencyCall> liveUpdateDependencyCalls;

  bool failNextActivate = false;
  std::vector<ShovelerComponent*> activateCalls;

  bool propagateNextUpdate = false;
  std::vector<std::pair<ShovelerComponent*, double>> updateCalls;

  std::vector<ShovelerComponent*> deactivateCalls;
};

MATCHER_P5(
    IsWorldUpdateComponentIntValueCall, component, fieldId, field, intValue, isCanonical, "") {
  const ShovelerComponentTest::OnUpdateComponentFieldCall& call = arg;
  return call.component == component && call.fieldId == fieldId && call.field == field &&
      call.value->type == SHOVELER_COMPONENT_FIELD_TYPE_INT && call.value->isSet &&
      call.value->intValue == intValue && call.isCanonical == isCanonical;
}

MATCHER_P4(IsLiveUpdateStringValueCall, component, fieldId, field, stringValue, "") {
  const ShovelerComponentTest::LiveUpdateCall& call = arg;
  return call.component == component && call.fieldId == fieldId && call.field == field &&
      call.value->type == SHOVELER_COMPONENT_FIELD_TYPE_STRING && call.value->isSet &&
      std::string(call.value->stringValue) == stringValue;
}

TEST_F(ShovelerComponentTest, activateDeactivate) {
  ASSERT_FALSE(shovelerComponentIsActive(component1));

  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  ASSERT_TRUE(shovelerComponentIsActive(component1));
  ASSERT_THAT(activateCalls, ElementsAre(component1));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component1));

  shovelerComponentDeactivate(component1);
  ASSERT_FALSE(shovelerComponentIsActive(component1));
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1));
}

TEST_F(ShovelerComponentTest, activateThroughDependencyActivation) {
  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);

  bool activated = shovelerComponentActivate(component1);
  ASSERT_FALSE(activated);
  ASSERT_FALSE(shovelerComponentIsActive(component1));

  bool dependencyActivatedBeforeDelegation = shovelerComponentActivate(component2);
  ASSERT_FALSE(dependencyActivatedBeforeDelegation);

  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  ASSERT_TRUE(shovelerComponentIsActive(component2));
  ASSERT_TRUE(shovelerComponentIsActive(component1))
      << "component 1 has also been activated after activating its dependency";
  ASSERT_THAT(activateCalls, ElementsAre(component2, component1));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component2, component1));
}

TEST_F(ShovelerComponentTest, deactivateWhenAddingUnsatisfiedDependency) {
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  ASSERT_TRUE(shovelerComponentIsActive(component1));
  ASSERT_THAT(activateCalls, ElementsAre(component1));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component1));

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);
  ASSERT_THAT(deactivateCalls, ElementsAre(component1))
      << "component 1 should be deactivated after adding unsatisfied dependency";
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1));
  ASSERT_FALSE(shovelerComponentIsActive(component1));
}

TEST_F(ShovelerComponentTest, deactivateReverseDependencies) {
  shovelerComponentActivate(component1);
  shovelerComponentDelegate(component2);
  shovelerComponentActivate(component2);

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);

  shovelerComponentDeactivate(component2);
  ASSERT_THAT(deactivateCalls, ElementsAre(component1, component2))
      << "component 1 has also been deactivated after deactivating its dependency";
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1, component2));
  ASSERT_FALSE(shovelerComponentIsActive(component1));
}

TEST_F(ShovelerComponentTest, deactivateFreedComponent) {
  shovelerComponentActivate(component1);
  shovelerComponentFree(component1);
  ASSERT_THAT(deactivateCalls, ElementsAre(component1));
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1));

  component1 = nullptr;
}

TEST_F(ShovelerComponentTest, updateField) {
  const int newFieldValue = 27;

  int firstValue = shovelerComponentGetFieldValueInt(component1, COMPONENT_TYPE_1_FIELD_PRIMITIVE);
  ASSERT_EQ(firstValue, 0);

  bool updated = shovelerComponentUpdateCanonicalFieldInt(
      component1, COMPONENT_TYPE_1_FIELD_PRIMITIVE, newFieldValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(
      onUpdateComponentFieldCalls,
      ElementsAre(IsWorldUpdateComponentIntValueCall(
          component1,
          COMPONENT_TYPE_1_FIELD_PRIMITIVE,
          &componentType1->fields[COMPONENT_TYPE_1_FIELD_PRIMITIVE],
          newFieldValue,
          /* isCanonical */ true)));

  int secondValue = shovelerComponentGetFieldValueInt(component1, COMPONENT_TYPE_1_FIELD_PRIMITIVE);
  ASSERT_EQ(secondValue, newFieldValue);
}

TEST_F(ShovelerComponentTest, updateFieldWithReactivation) {
  const int newFieldValue = 27;

  shovelerComponentActivate(component1);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  bool updated = shovelerComponentUpdateCanonicalFieldInt(
      component1, COMPONENT_TYPE_1_FIELD_PRIMITIVE, newFieldValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(1));
  ASSERT_THAT(liveUpdateCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, ElementsAre(component1));
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1));
  ASSERT_THAT(activateCalls, ElementsAre(component1));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component1));
}

TEST_F(ShovelerComponentTest, updateConfigurationLive) {
  const char* newFieldValue = "new value";

  shovelerComponentDelegate(component2);
  shovelerComponentActivate(component2);
  ASSERT_THAT(activateCalls, ElementsAre(component2));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component2));
  activateCalls.clear();
  onActivateComponentCalls.clear();

  bool updated = shovelerComponentUpdateCanonicalFieldString(
      component2, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE, newFieldValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(1));
  ASSERT_THAT(
      liveUpdateCalls,
      ElementsAre(IsLiveUpdateStringValueCall(
          component2,
          COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
          &component2->type->fields[COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE],
          newFieldValue)));
  ASSERT_THAT(activateCalls, IsEmpty());
  ASSERT_THAT(onActivateComponentCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());

  const char* newValue = shovelerComponentGetFieldValueString(
      component2, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE);
  ASSERT_STREQ(newValue, newFieldValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveUpdatesReverseDependency) {
  const char* newConfigurationValue = "new value";

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextLiveUpdate = true;
  bool updated = shovelerComponentUpdateCanonicalFieldString(
      component2, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE, newConfigurationValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(2));
  ASSERT_THAT(
      liveUpdateDependencyCalls,
      ElementsAre(LiveUpdateDependencyCall{
          component1,
          COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
          &component1->type->fields[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE],
          component2}));
  ASSERT_THAT(activateCalls, IsEmpty());
  ASSERT_THAT(onActivateComponentCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveWithoutPropagation) {
  const char* newConfigurationValue = "new value";

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextLiveUpdate = false;
  bool updated = shovelerComponentUpdateCanonicalFieldString(
      component2, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE, newConfigurationValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(2));
  ASSERT_THAT(liveUpdateDependencyCalls, IsEmpty());
  ASSERT_THAT(activateCalls, IsEmpty());
  ASSERT_THAT(onActivateComponentCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());
}

TEST_F(ShovelerComponentTest, updateComponentUpdatesReverseDependency) {
  double dt = 1234.5;

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextUpdate = true;
  bool updatePropagated = shovelerComponentUpdate(component2, dt);
  ASSERT_TRUE(updatePropagated);
  ASSERT_THAT(updateCalls, ElementsAre(std::make_pair(component2, dt)));
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(1));
  ASSERT_THAT(
      liveUpdateDependencyCalls,
      ElementsAre(LiveUpdateDependencyCall{
          component1,
          COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
          &component1->type->fields[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE],
          component2}));
  ASSERT_THAT(activateCalls, IsEmpty());
  ASSERT_THAT(onActivateComponentCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveReactivatesReverseDependency) {
  const char* newConfigurationValue = "new value";

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextLiveUpdate = true;
  bool updated = shovelerComponentUpdateCanonicalFieldString(
      component2, COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE, newConfigurationValue);
  ASSERT_TRUE(updated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(2));
  ASSERT_THAT(liveUpdateDependencyCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, ElementsAre(component1));
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1));
  ASSERT_THAT(activateCalls, ElementsAre(component1));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component1));
}

TEST_F(ShovelerComponentTest, updateComponentReactivatesReverseDependency) {
  double dt = 1234.5;

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextUpdate = true;
  bool updatePropagated = shovelerComponentUpdate(component2, dt);
  ASSERT_TRUE(updatePropagated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(1));
  ASSERT_THAT(liveUpdateDependencyCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, ElementsAre(component1));
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component1));
  ASSERT_THAT(activateCalls, ElementsAre(component1));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component1));
}

TEST_F(ShovelerComponentTest, nonPropagatingUpdateComponentDoesntAffectReverseDependency) {
  double dt = 1234.5;

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId2);
  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextUpdate = false;
  bool updatePropagated = shovelerComponentUpdate(component2, dt);
  ASSERT_FALSE(updatePropagated);
  ASSERT_THAT(updateCalls, ElementsAre(std::make_pair(component2, dt)));
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(2));
  ASSERT_THAT(liveUpdateDependencyCalls, IsEmpty());
  ASSERT_THAT(activateCalls, IsEmpty());
  ASSERT_THAT(onActivateComponentCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());
}

TEST_F(ShovelerComponentTest, doublePropagateUpdate) {
  double dt = 1234.5;

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);

  shovelerComponentUpdateCanonicalFieldEntityId(
      component3, COMPONENT_TYPE_3_FIELD_DEPENDENCY, entityId1);
  bool secondDependencyActivated = shovelerComponentActivate(component3);
  ASSERT_TRUE(secondDependencyActivated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextUpdate = true;
  propagateNextLiveUpdateDependency = true;
  bool updatePropagated = shovelerComponentUpdate(component2, dt);
  ASSERT_TRUE(updatePropagated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(2));
  ASSERT_THAT(
      liveUpdateDependencyCalls,
      ElementsAre(LiveUpdateDependencyCall{
          component1,
          COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
          &component1->type->fields[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE],
          component2}));
  ASSERT_THAT(deactivateCalls, ElementsAre(component3));
  ASSERT_THAT(onDeactivateComponentCalls, ElementsAre(component3));
  ASSERT_THAT(activateCalls, ElementsAre(component3));
  ASSERT_THAT(onActivateComponentCalls, ElementsAre(component3));
}

TEST_F(ShovelerComponentTest, dontPropagateLiveDependencyUpdate) {
  double dt = 1234.5;

  shovelerComponentUpdateCanonicalFieldEntityId(
      component1, COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE, entityId2);
  shovelerComponentDelegate(component2);
  bool dependencyActivated = shovelerComponentActivate(component2);
  ASSERT_TRUE(dependencyActivated);
  bool activated = shovelerComponentActivate(component1);
  ASSERT_TRUE(activated);

  shovelerComponentUpdateCanonicalFieldEntityId(
      component3, COMPONENT_TYPE_3_FIELD_DEPENDENCY, entityId1);
  bool secondDependencyActivated = shovelerComponentActivate(component3);
  ASSERT_TRUE(secondDependencyActivated);
  activateCalls.clear();
  onActivateComponentCalls.clear();

  propagateNextUpdate = true;
  propagateNextLiveUpdateDependency = false;
  bool updatePropagated = shovelerComponentUpdate(component2, dt);
  ASSERT_TRUE(updatePropagated);
  ASSERT_THAT(onUpdateComponentFieldCalls, SizeIs(2));
  ASSERT_THAT(
      liveUpdateDependencyCalls,
      ElementsAre(LiveUpdateDependencyCall{
          component1,
          COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
          &component1->type->fields[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE],
          component2}));
  ASSERT_THAT(activateCalls, IsEmpty());
  ASSERT_THAT(onActivateComponentCalls, IsEmpty());
  ASSERT_THAT(deactivateCalls, IsEmpty());
  ASSERT_THAT(onDeactivateComponentCalls, IsEmpty());
}

static ShovelerComponent* getComponent(
    ShovelerComponent* component,
    long long int entityId,
    const char* componentTypeId,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  if (entityId == entityId1 && componentTypeId == componentType1Id) {
    return test->component1;
  }

  if (entityId == entityId2 && componentTypeId == componentType2Id) {
    return test->component2;
  }

  if (entityId == entityId1 && componentTypeId == componentType3Id) {
    return test->component3;
  }

  return nullptr;
}

static void onUpdateComponentField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    const ShovelerComponentFieldValue* value,
    bool isCanonical,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->onUpdateComponentFieldCalls.push_back(ShovelerComponentTest::OnUpdateComponentFieldCall{
      component, fieldId, field, value, isCanonical});
}

static void onActivateComponent(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->onActivateComponentCalls.emplace_back(component);
}

static void onDeactivateComponent(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->onDeactivateComponentCalls.emplace_back(component);
}

static void addDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  auto dependencyTarget = std::make_pair(targetEntityId, targetComponentTypeId);
  test->reverseDependencies[dependencyTarget].insert(component);
}

static bool removeDependency(
    ShovelerComponent* component,
    long long int targetEntityId,
    const char* targetComponentTypeId,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  auto dependencyTarget = std::make_pair(targetEntityId, targetComponentTypeId);
  test->reverseDependencies[dependencyTarget].erase(component);
  return true;
}

static void forEachReverseDependency(
    ShovelerComponent* component,
    ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction* callbackFunction,
    void* callbackUserData,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  auto dependencyTarget = std::make_pair(component->entityId, component->type->id);
  for (ShovelerComponent* sourceComponent : test->reverseDependencies[dependencyTarget]) {
    if (sourceComponent != nullptr) {
      callbackFunction(sourceComponent, component, callbackUserData);
    }
  }
}

static bool requiresAuthority(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  if (component->type == test->componentType2) {
    return true;
  }

  return false;
}

static bool canLiveUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  if (component->type == test->componentType2 &&
      field->name == componentType2FieldPrimitiveLiveUpdate) {
    return true;
  }

  return false;
}

static bool canLiveUpdateDependencyField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);

  if (component->type == test->componentType1 &&
      field->name == componentType1FieldDependencyLiveUpdate) {
    return true;
  }

  return false;
}

static bool liveUpdateField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponentFieldValue* fieldValue,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->liveUpdateCalls.push_back(
      ShovelerComponentTest::LiveUpdateCall{component, fieldId, field, fieldValue});
  return test->propagateNextLiveUpdate;
}

static bool liveUpdateDependencyField(
    ShovelerComponent* component,
    int fieldId,
    const ShovelerComponentField* field,
    ShovelerComponent* dependencyComponent,
    void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->liveUpdateDependencyCalls.emplace_back(ShovelerComponentTest::LiveUpdateDependencyCall{
      component, fieldId, field, dependencyComponent});
  return test->propagateNextLiveUpdateDependency;
}

static void* activateComponent(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->activateCalls.emplace_back(component);

  if (test->failNextActivate) {
    return nullptr;
  }

  return test;
}

static bool updateComponent(ShovelerComponent* component, double dt, void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->updateCalls.emplace_back(component, dt);
  return test->propagateNextUpdate;
}

static void deactivateComponent(ShovelerComponent* component, void* testPointer) {
  auto* test = static_cast<ShovelerComponentTest*>(testPointer);
  test->deactivateCalls.emplace_back(component);
}
