#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
#include "shoveler/view.h"
}

static const char *testTargetName = "test";

static void *activateComponent(ShovelerComponent *component);
static void deactivateComponent(ShovelerComponent *component);
static void liveUpdateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateDependencyComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);
static void dependencyCallbackFunction(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencySource, const ShovelerViewQualifiedComponent *dependencyTarget, bool added, void *testPointer);

class ShovelerViewTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		view = shovelerViewCreate();
		shovelerViewSetTarget(view, testTargetName, this);

		ShovelerComponentType *componentType = shovelerComponentTypeCreate(componentTypeName, activateComponent, deactivateComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, configurationOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, dependencyConfigurationOptionKey, otherComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, updateDependencyComponent);
		bool componentTypeAdded = shovelerViewAddComponentType(view, componentType);
		ASSERT_TRUE(componentTypeAdded);

		ShovelerComponentType *otherComponentType = shovelerComponentTypeCreate(otherComponentTypeName, NULL, NULL, /* requiresAuthority */ true);
		shovelerComponentTypeAddConfigurationOption(otherComponentType, liveUpdateConfigurationOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, liveUpdateComponent);
		bool otherComponentTypeAdded = shovelerViewAddComponentType(view, otherComponentType);
		ASSERT_TRUE(otherComponentTypeAdded);

		activateCalled = false;
		deactivateCalled = false;
		liveUpdateCalled = false;
		lastLiveUpdateConfigurationOption = NULL;
		lastLiveUpdateValue = NULL;
		lastDependencySource.entityId = 0;
		lastDependencySource.componentTypeName = NULL;
		lastDependencyTarget.entityId = 0;
		lastDependencyTarget.componentTypeName = NULL;
		lastDependencyAdded = false;
	}

	virtual void TearDown()
	{
		clearDependencyCallback();
		shovelerViewFree(view);
	}

	void clearDependencyCallback()
	{
		free(lastDependencySource.componentTypeName);
		free(lastDependencyTarget.componentTypeName);

		lastDependencySource.entityId = 0;
		lastDependencySource.componentTypeName = NULL;
		lastDependencyTarget.entityId = 0;
		lastDependencyTarget.componentTypeName = NULL;
		lastDependencyAdded = false;
	}

	const char *componentTypeName = "component type";
	const char *otherComponentTypeName = "other component type";

	const char *configurationOptionKey = "configuration option key";
	const char *dependencyConfigurationOptionKey = "dependency configuration option key";
	const char *liveUpdateConfigurationOptionKey = "live update configuration option key";

	ShovelerView *view;

	bool activateCalled;
	bool deactivateCalled;
	bool liveUpdateCalled;
	bool updateDependencyCalled;
	ShovelerComponentTypeConfigurationOption *lastLiveUpdateConfigurationOption;
	const ShovelerComponentConfigurationValue *lastLiveUpdateValue;
	ShovelerComponentTypeConfigurationOption *lastUpdateDependencyConfigurationOption;
	ShovelerComponent *lastUpdateDependencyComponent;
	ShovelerViewQualifiedComponent lastDependencySource;
	ShovelerViewQualifiedComponent lastDependencyTarget;
	bool lastDependencyAdded;
};

TEST_F(ShovelerViewTest, activateDeactivate)
{
	long long int testEntityId = 1337;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));

	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_TRUE(activated);
	ASSERT_TRUE(shovelerComponentIsActive(testComponent));
	ASSERT_TRUE(activateCalled);

	shovelerComponentDeactivate(testComponent);
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));
}

TEST_F(ShovelerViewTest, activateThroughDependencyActivation)
{
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, dependencyConfigurationOptionKey, testDependencyEntityId);

	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_FALSE(activated);
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);

	ShovelerComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, otherComponentTypeName);
	ASSERT_TRUE(testDependencyComponent != NULL);

	bool dependencyActivatedBeforeDelegation = shovelerComponentActivate(testDependencyComponent);
	ASSERT_FALSE(dependencyActivatedBeforeDelegation);

	shovelerViewEntityDelegate(testDependencyEntity, otherComponentTypeName);
	bool dependencyActivated = shovelerComponentActivate(testDependencyComponent);
	ASSERT_TRUE(dependencyActivated);
	ASSERT_TRUE(shovelerComponentIsActive(testDependencyComponent));
	ASSERT_TRUE(shovelerComponentIsActive(testComponent)) << "test component has also been activated after activating its dependency";
	ASSERT_TRUE(activateCalled);
}

TEST_F(ShovelerViewTest, deactivateWhenAddingUnsatisfiedDependency)
{
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);

	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_TRUE(activated);
	ASSERT_TRUE(shovelerComponentIsActive(testComponent));
	ASSERT_TRUE(activateCalled);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, dependencyConfigurationOptionKey, testDependencyEntityId);
	ASSERT_TRUE(deactivateCalled) << "test component should be deactivated after adding unsatisfied dependency";
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));
}

TEST_F(ShovelerViewTest, deactivateReverseDependencies)
{
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);
	shovelerComponentActivate(testComponent);

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);
	ShovelerComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, otherComponentTypeName);
	ASSERT_TRUE(testDependencyComponent != NULL);
	shovelerComponentActivate(testDependencyComponent);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, dependencyConfigurationOptionKey, testDependencyEntityId);

	shovelerComponentDeactivate(testDependencyComponent);
	ASSERT_TRUE(deactivateCalled) << "test component has also been deactivated after deactivating its dependency";
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));
}

TEST_F(ShovelerViewTest, deactivateRemovedComponent)
{
	long long int testEntityId = 1337;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);
	shovelerComponentActivate(testComponent);

	bool removed = shovelerViewEntityRemoveComponent(testEntity, componentTypeName);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerViewTest, removeEntityRemovesComponents)
{
	long long int testEntityId = 1337;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);
	shovelerComponentActivate(testComponent);

	bool removed = shovelerViewRemoveEntity(view, testEntityId);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerViewTest, updateConfiguration) {
	long long int testEntityId = 1337;
	const int newConfigurationValue = 27;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);

	int firstValue = shovelerComponentGetConfigurationValueInt(testComponent, configurationOptionKey);
	ASSERT_EQ(firstValue, 0);

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionInt(testComponent, configurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);

	int secondValue = shovelerComponentGetConfigurationValueInt(testComponent, configurationOptionKey);
	ASSERT_EQ(secondValue, newConfigurationValue);
}

TEST_F(ShovelerViewTest, updateConfigurationWithReactivation) {
	long long int testEntityId = 1337;
	const int newConfigurationValue = 27;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);

	shovelerComponentActivate(testComponent);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionInt(testComponent, configurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_FALSE(liveUpdateCalled);
	ASSERT_TRUE(activateCalled);
	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerViewTest, updateConfigurationLive) {
	long long int testEntityId = 1337;
	const char *newConfigurationValue = "new value";

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, otherComponentTypeName);
	ASSERT_TRUE(testComponent != NULL);

	shovelerViewEntityDelegate(testEntity, otherComponentTypeName);
	shovelerComponentActivate(testComponent);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(testComponent, liveUpdateConfigurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(liveUpdateCalled);
	ASSERT_STREQ(lastLiveUpdateConfigurationOption->key, liveUpdateConfigurationOptionKey);
	ASSERT_STREQ(lastLiveUpdateValue->stringValue, newConfigurationValue);
	ASSERT_FALSE(activateCalled);
	ASSERT_FALSE(deactivateCalled);

	const char *newValue = shovelerComponentGetConfigurationValueString(testComponent, liveUpdateConfigurationOptionKey);
	ASSERT_STREQ(newValue, newConfigurationValue);
}

TEST_F(ShovelerViewTest, updateConfigurationLiveNotifiesReverseDependency) {
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;
	const char *newConfigurationValue = "new value";

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeName);
	ASSERT_TRUE(testComponent != NULL);

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);
	ShovelerComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, otherComponentTypeName);
	ASSERT_TRUE(testDependencyComponent != NULL);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, dependencyConfigurationOptionKey, testDependencyEntityId);
	shovelerViewEntityDelegate(testDependencyEntity, otherComponentTypeName);
	bool dependencyActivated = shovelerComponentActivate(testDependencyComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_TRUE(activated);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(testDependencyComponent, liveUpdateConfigurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateDependencyCalled);
	ASSERT_STREQ(lastUpdateDependencyConfigurationOption->key, dependencyConfigurationOptionKey);
	ASSERT_EQ(lastUpdateDependencyComponent, testDependencyComponent);
}

static void *activateComponent(ShovelerComponent *component)
{
	ShovelerViewTest *test = (ShovelerViewTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->activateCalled = true;
	return test;
}

static void deactivateComponent(ShovelerComponent *component)
{
	ShovelerViewTest *test = (ShovelerViewTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->deactivateCalled = true;
}

static void liveUpdateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	ShovelerViewTest *test = (ShovelerViewTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->liveUpdateCalled = true;
	test->lastLiveUpdateConfigurationOption = configurationOption;
	test->lastLiveUpdateValue = value;
}

static void updateDependencyComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerViewTest *test = (ShovelerViewTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->updateDependencyCalled = true;
	test->lastUpdateDependencyConfigurationOption = configurationOption;
	test->lastUpdateDependencyComponent = dependencyComponent;
}

static void dependencyCallbackFunction(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencySource, const ShovelerViewQualifiedComponent *dependencyTarget, bool added, void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) testPointer;
	test->lastDependencySource.entityId = dependencySource->entityId;
	test->lastDependencySource.componentTypeName = strdup(dependencySource->componentTypeName);
	test->lastDependencyTarget.entityId = dependencyTarget->entityId;
	test->lastDependencyTarget.componentTypeName = strdup(dependencyTarget->componentTypeName);
	test->lastDependencyAdded = added;
}
