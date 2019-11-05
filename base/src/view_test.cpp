#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
#include "shoveler/view.h"
}

typedef enum {
	COMPONENT_CONFIGURATION_OPTION,
	COMPONENT_CONFIGURATION_OPTION_DEPENDENCY,
} ComponentConfigurationOption;

typedef enum {
	COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE,
} OtherComponentConfigurationOption;

static const char *testTargetName = "test";

static void *activateComponent(ShovelerComponent *component);
static void deactivateComponent(ShovelerComponent *component);
static void liveUpdateComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateDependencyComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

class ShovelerViewTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		view = shovelerViewCreate(NULL, NULL);
		shovelerViewSetTarget(view, testTargetName, this);

		configurationOptions[COMPONENT_CONFIGURATION_OPTION] = shovelerComponentTypeConfigurationOption(configurationOptionName, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		configurationOptions[COMPONENT_CONFIGURATION_OPTION_DEPENDENCY] = shovelerComponentTypeConfigurationOptionDependency(dependencyConfigurationOptionName, otherComponentTypeId, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, updateDependencyComponent);

		otherConfigurationOptions[COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE] = shovelerComponentTypeConfigurationOption(liveUpdateConfigurationOptionName, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, liveUpdateComponent);

		ShovelerComponentType *componentType = shovelerComponentTypeCreate(componentTypeId, activateComponent, deactivateComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
		bool componentTypeAdded = shovelerViewAddComponentType(view, componentType);
		ASSERT_TRUE(componentTypeAdded);

		ShovelerComponentType *otherComponentType = shovelerComponentTypeCreate(otherComponentTypeId, NULL, NULL, /* requiresAuthority */ true, sizeof(otherConfigurationOptions) / sizeof(otherConfigurationOptions[0]), otherConfigurationOptions);
		bool otherComponentTypeAdded = shovelerViewAddComponentType(view, otherComponentType);
		ASSERT_TRUE(otherComponentTypeAdded);

		activateCalled = false;
		deactivateCalled = false;
		liveUpdateCalled = false;
		lastLiveUpdateConfigurationOption = NULL;
		lastLiveUpdateValue = NULL;
		lastDependencySource.entityId = 0;
		lastDependencySource.componentTypeId = NULL;
		lastDependencyTarget.entityId = 0;
		lastDependencyTarget.componentTypeId = NULL;
		lastDependencyAdded = false;
	}

	virtual void TearDown()
	{
		clearDependencyCallback();
		shovelerViewFree(view);
	}

	void clearDependencyCallback()
	{
		lastDependencySource.entityId = 0;
		lastDependencySource.componentTypeId = NULL;
		lastDependencyTarget.entityId = 0;
		lastDependencyTarget.componentTypeId = NULL;
		lastDependencyAdded = false;
	}

	const char *componentTypeId = "component type";
	const char *otherComponentTypeId = "other component type";

	const char *configurationOptionName = "option";
	const char *dependencyConfigurationOptionName = "dependency";
	const char *liveUpdateConfigurationOptionName = "live update";

	ShovelerView *view;

	ShovelerComponentTypeConfigurationOption configurationOptions[2];
	ShovelerComponentTypeConfigurationOption otherConfigurationOptions[1];

	bool activateCalled;
	bool deactivateCalled;
	bool liveUpdateCalled;
	bool updateDependencyCalled;
	const ShovelerComponentTypeConfigurationOption *lastLiveUpdateConfigurationOption;
	const ShovelerComponentConfigurationValue *lastLiveUpdateValue;
	const ShovelerComponentTypeConfigurationOption *lastUpdateDependencyConfigurationOption;
	ShovelerComponent *lastUpdateDependencyComponent;
	ShovelerViewQualifiedComponent lastDependencySource;
	ShovelerViewQualifiedComponent lastDependencyTarget;
	bool lastDependencyAdded;
};

TEST_F(ShovelerViewTest, activateThroughDependencyActivation)
{
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeId);
	ASSERT_TRUE(testComponent != NULL);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, testDependencyEntityId);

	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_FALSE(activated);
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);

	ShovelerComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, otherComponentTypeId);
	ASSERT_TRUE(testDependencyComponent != NULL);

	bool dependencyActivatedBeforeDelegation = shovelerComponentActivate(testDependencyComponent);
	ASSERT_FALSE(dependencyActivatedBeforeDelegation);

	shovelerViewEntityDelegate(testDependencyEntity, otherComponentTypeId);
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

	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeId);
	ASSERT_TRUE(testComponent != NULL);

	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_TRUE(activated);
	ASSERT_TRUE(shovelerComponentIsActive(testComponent));
	ASSERT_TRUE(activateCalled);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, testDependencyEntityId);
	ASSERT_TRUE(deactivateCalled) << "test component should be deactivated after adding unsatisfied dependency";
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));
}

TEST_F(ShovelerViewTest, deactivateReverseDependencies)
{
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeId);
	ASSERT_TRUE(testComponent != NULL);
	shovelerComponentActivate(testComponent);

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);
	ShovelerComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, otherComponentTypeId);
	ASSERT_TRUE(testDependencyComponent != NULL);
	shovelerComponentActivate(testDependencyComponent);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, testDependencyEntityId);

	shovelerComponentDeactivate(testDependencyComponent);
	ASSERT_TRUE(deactivateCalled) << "test component has also been deactivated after deactivating its dependency";
	ASSERT_FALSE(shovelerComponentIsActive(testComponent));
}

TEST_F(ShovelerViewTest, deactivateRemovedComponent)
{
	long long int testEntityId = 1337;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeId);
	ASSERT_TRUE(testComponent != NULL);
	shovelerComponentActivate(testComponent);

	bool removed = shovelerViewEntityRemoveComponent(testEntity, componentTypeId);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerViewTest, removeEntityRemovesComponents)
{
	long long int testEntityId = 1337;

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeId);
	ASSERT_TRUE(testComponent != NULL);
	shovelerComponentActivate(testComponent);

	bool removed = shovelerViewRemoveEntity(view, testEntityId);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerViewTest, updateConfigurationLiveNotifiesReverseDependency) {
	long long int testEntityId = 1337;
	long long int testDependencyEntityId = 42;
	const char *newConfigurationValue = "new value";

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);
	ShovelerComponent *testComponent = shovelerViewEntityAddComponent(testEntity, componentTypeId);
	ASSERT_TRUE(testComponent != NULL);

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);
	ShovelerComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, otherComponentTypeId);
	ASSERT_TRUE(testDependencyComponent != NULL);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(testComponent, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, testDependencyEntityId);
	shovelerViewEntityDelegate(testDependencyEntity, otherComponentTypeId);
	bool dependencyActivated = shovelerComponentActivate(testDependencyComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(testComponent);
	ASSERT_TRUE(activated);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(testDependencyComponent, COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateDependencyCalled);
	ASSERT_EQ(lastUpdateDependencyConfigurationOption->name, dependencyConfigurationOptionName);
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

static void liveUpdateComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	ShovelerViewTest *test = (ShovelerViewTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->liveUpdateCalled = true;
	test->lastLiveUpdateConfigurationOption = configurationOption;
	test->lastLiveUpdateValue = value;
}

static void updateDependencyComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerViewTest *test = (ShovelerViewTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->updateDependencyCalled = true;
	test->lastUpdateDependencyConfigurationOption = configurationOption;
	test->lastUpdateDependencyComponent = dependencyComponent;
}
