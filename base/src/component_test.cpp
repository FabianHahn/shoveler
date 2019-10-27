#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
}

static const char *testTargetName = "test";

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentName, void *testPointer);
static void *getTarget(ShovelerComponent *component, const char *targetName, void *testPointer);
static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName, void *testPointer);
static bool removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName, void *testPointer);
static void forEachReverseDependency(ShovelerComponent *component, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *testPointer);
static void reportActivation(ShovelerComponent *component, int delta, void *testPointer);

static void *activateComponent(ShovelerComponent *component);
static void deactivateComponent(ShovelerComponent *component);
static void liveUpdateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static void updateDependencyComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

class ShovelerComponentTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		viewAdapter.getComponent = getComponent;
		viewAdapter.getTarget = getTarget;
		viewAdapter.addDependency = addDependency;
		viewAdapter.removeDependency = removeDependency;
		viewAdapter.forEachReverseDependency = forEachReverseDependency;
		viewAdapter.reportActivation = reportActivation;
		viewAdapter.userData = this;

		componentType = shovelerComponentTypeCreate(componentTypeName, activateComponent, deactivateComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, configurationOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, dependencyConfigurationOptionKey, otherComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, updateDependencyComponent);

		otherComponentType = shovelerComponentTypeCreate(otherComponentTypeName, /* activate */ NULL, /* deactivate */ NULL, /* requiresAuthority */ true);
		shovelerComponentTypeAddConfigurationOption(otherComponentType, liveUpdateConfigurationOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, liveUpdateComponent);

		component = shovelerComponentCreate(&viewAdapter, entityId, componentType);
		ASSERT_TRUE(component != NULL);

		otherComponent = shovelerComponentCreate(&viewAdapter, otherEntityId, otherComponentType);
		ASSERT_TRUE(otherComponent != NULL);

		activateCalled = false;
		deactivateCalled = false;
		liveUpdateCalled = false;
		lastLiveUpdateConfigurationOption = NULL;
		lastLiveUpdateValue = NULL;
	}

	virtual void TearDown()
	{
		shovelerComponentFree(otherComponent);
		shovelerComponentFree(component);
		shovelerComponentTypeFree(componentType);
		shovelerComponentTypeFree(otherComponentType);
	}

	const char *componentTypeName = "component type";
	const char *otherComponentTypeName = "other component type";

	const char *configurationOptionKey = "configuration option key";
	const char *dependencyConfigurationOptionKey = "dependency configuration option key";
	const char *liveUpdateConfigurationOptionKey = "live update configuration option key";

	const long long int entityId = 1;
	const long long int otherEntityId = 2;

	ShovelerComponentViewAdapter viewAdapter;
	ShovelerComponentType *componentType;
	ShovelerComponentType *otherComponentType;
	ShovelerComponent *component;
	ShovelerComponent *otherComponent;

	std::map<std::pair<long long int, std::string>, std::set<ShovelerComponent *> > reverseDependencies;

	bool activateCalled;
	bool deactivateCalled;
	bool liveUpdateCalled;
	bool updateDependencyCalled;
	ShovelerComponentTypeConfigurationOption *lastLiveUpdateConfigurationOption;
	const ShovelerComponentConfigurationValue *lastLiveUpdateValue;
	ShovelerComponentTypeConfigurationOption *lastUpdateDependencyConfigurationOption;
	ShovelerComponent *lastUpdateDependencyComponent;
};

TEST_F(ShovelerComponentTest, activateDeactivate)
{
	ASSERT_FALSE(shovelerComponentIsActive(component));

	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	ASSERT_TRUE(shovelerComponentIsActive(component));
	ASSERT_TRUE(activateCalled);

	shovelerComponentDeactivate(component);
	ASSERT_FALSE(shovelerComponentIsActive(component));
}

TEST_F(ShovelerComponentTest, activateThroughDependencyActivation)
{
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, dependencyConfigurationOptionKey, otherEntityId);

	bool activated = shovelerComponentActivate(component);
	ASSERT_FALSE(activated);
	ASSERT_FALSE(shovelerComponentIsActive(component));

	bool dependencyActivatedBeforeDelegation = shovelerComponentActivate(otherComponent);
	ASSERT_FALSE(dependencyActivatedBeforeDelegation);

	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	ASSERT_TRUE(shovelerComponentIsActive(otherComponent));
	ASSERT_TRUE(shovelerComponentIsActive(component)) << "test component has also been activated after activating its dependency";
	ASSERT_TRUE(activateCalled);
}

TEST_F(ShovelerComponentTest, deactivateWhenAddingUnsatisfiedDependency)
{
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	ASSERT_TRUE(shovelerComponentIsActive(component));
	ASSERT_TRUE(activateCalled);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, dependencyConfigurationOptionKey, otherEntityId);
	ASSERT_TRUE(deactivateCalled) << "test component should be deactivated after adding unsatisfied dependency";
	ASSERT_FALSE(shovelerComponentIsActive(component));
}

TEST_F(ShovelerComponentTest, deactivateReverseDependencies)
{
	shovelerComponentActivate(component);
	shovelerComponentActivate(otherComponent);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, dependencyConfigurationOptionKey, otherEntityId);

	shovelerComponentDeactivate(otherComponent);
	ASSERT_TRUE(deactivateCalled) << "test component has also been deactivated after deactivating its dependency";
	ASSERT_FALSE(shovelerComponentIsActive(component));
}

TEST_F(ShovelerComponentTest, deactivateFreedComponent)
{
	shovelerComponentActivate(component);
	shovelerComponentFree(component);
	component = NULL;

	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerComponentTest, updateConfiguration) {
	const int newConfigurationValue = 27;

	int firstValue = shovelerComponentGetConfigurationValueInt(component, configurationOptionKey);
	ASSERT_EQ(firstValue, 0);

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionInt(component, configurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);

	int secondValue = shovelerComponentGetConfigurationValueInt(component, configurationOptionKey);
	ASSERT_EQ(secondValue, newConfigurationValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationWithReactivation) {
	const int newConfigurationValue = 27;

	shovelerComponentActivate(component);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionInt(component, configurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_FALSE(liveUpdateCalled);
	ASSERT_TRUE(activateCalled);
	ASSERT_TRUE(deactivateCalled);
}

TEST_F(ShovelerComponentTest, updateConfigurationLive) {
	const char *newConfigurationValue = "new value";

	shovelerComponentDelegate(otherComponent);
	shovelerComponentActivate(otherComponent);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(otherComponent, liveUpdateConfigurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(liveUpdateCalled);
	ASSERT_STREQ(lastLiveUpdateConfigurationOption->key, liveUpdateConfigurationOptionKey);
	ASSERT_STREQ(lastLiveUpdateValue->stringValue, newConfigurationValue);
	ASSERT_FALSE(activateCalled);
	ASSERT_FALSE(deactivateCalled);

	const char *newValue = shovelerComponentGetConfigurationValueString(otherComponent, liveUpdateConfigurationOptionKey);
	ASSERT_STREQ(newValue, newConfigurationValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveNotifiesReverseDependency) {
	const char *newConfigurationValue = "new value";

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, dependencyConfigurationOptionKey, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(otherComponent, liveUpdateConfigurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateDependencyCalled);
	ASSERT_STREQ(lastUpdateDependencyConfigurationOption->key, dependencyConfigurationOptionKey);
	ASSERT_EQ(lastUpdateDependencyComponent, otherComponent);
}

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentTypeName, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	if(entityId == test->entityId && strcmp(componentTypeName, test->componentTypeName) == 0) {
		return test->component;
	}

	if(entityId == test->otherEntityId && strcmp(componentTypeName, test->otherComponentTypeName) == 0) {
		return test->otherComponent;
	}

	return NULL;
}

static void *getTarget(ShovelerComponent *component, const char *targetName, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	if(strcmp(targetName, testTargetName) == 0) {
		return test;
	}

	return NULL;
}

static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	auto dependencyTarget = std::make_pair(targetEntityId, targetComponentTypeName);
	test->reverseDependencies[dependencyTarget].insert(component);
}

static bool removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeName, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	auto dependencyTarget = std::make_pair(targetEntityId, targetComponentTypeName);
	test->reverseDependencies[dependencyTarget].erase(component);
	return true;
}

static void forEachReverseDependency(ShovelerComponent *component, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	auto dependencyTarget = std::make_pair(component->entityId, component->type->name);
	for(ShovelerComponent *sourceComponent : test->reverseDependencies[dependencyTarget]) {
		if(sourceComponent != NULL) {
			callbackFunction(sourceComponent, component, callbackUserData);
		}
	}
}

static void reportActivation(ShovelerComponent *component, int delta, void *testPointer)
{

}

static void *activateComponent(ShovelerComponent *component)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->activateCalled = true;
	return test;
}

static void deactivateComponent(ShovelerComponent *component)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->deactivateCalled = true;
}

static void liveUpdateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->liveUpdateCalled = true;
	test->lastLiveUpdateConfigurationOption = configurationOption;
	test->lastLiveUpdateValue = value;
}

static void updateDependencyComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->updateDependencyCalled = true;
	test->lastUpdateDependencyConfigurationOption = configurationOption;
	test->lastUpdateDependencyComponent = dependencyComponent;
}
