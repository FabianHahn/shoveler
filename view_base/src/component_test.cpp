#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
#include "shoveler/log.h"
}

typedef enum {
	COMPONENT_CONFIGURATION_OPTION,
	COMPONENT_CONFIGURATION_OPTION_DEPENDENCY,
	COMPONENT_CONFIGURATION_OPTION_REACTIVATE_DEPENDENCY,
} ComponentConfigurationOption;

typedef enum {
	OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE,
} OtherComponentConfigurationOption;

typedef enum {
	YET_ANOTHER_COMPONENT_CONFIGURATION_OPTION_DEPENDENCY,
} YetAnotherComponentConfigurationOption;

static const char *testTargetName = "test";

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentName, void *testPointer);
static void *getTarget(ShovelerComponent *component, const char *targetName, void *testPointer);
static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *testPointer);
static bool removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *testPointer);
static void forEachReverseDependency(ShovelerComponent *component, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *testPointer);
static void reportActivation(ShovelerComponent *component, int delta, void *testPointer);

static void *activateComponent(ShovelerComponent *component);
static bool updateComponent(ShovelerComponent *component, double dt);
static void deactivateComponent(ShovelerComponent *component);
static bool liveUpdateComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value);
static bool liveUpdateDependencyComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

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

		configurationOptions[COMPONENT_CONFIGURATION_OPTION] = shovelerComponentTypeConfigurationOption(configurationOptionName, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		configurationOptions[COMPONENT_CONFIGURATION_OPTION_DEPENDENCY] = shovelerComponentTypeConfigurationOptionDependency(dependencyConfigurationOptionName, otherComponentTypeId, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, liveUpdateDependencyComponent);
		configurationOptions[COMPONENT_CONFIGURATION_OPTION_REACTIVATE_DEPENDENCY] = shovelerComponentTypeConfigurationOptionDependency(dependencyConfigurationOptionName, otherComponentTypeId, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

		otherConfigurationOptions[OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE] = shovelerComponentTypeConfigurationOption(liveUpdateConfigurationOptionName, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_STRING, /* isOptional */ false, liveUpdateComponent);

		yetAnotherConfigurationOptions[YET_ANOTHER_COMPONENT_CONFIGURATION_OPTION_DEPENDENCY] = shovelerComponentTypeConfigurationOptionDependency(dependencyConfigurationOptionName, componentTypeId, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

		componentType = shovelerComponentTypeCreate(componentTypeId, activateComponent, /* update */ NULL, deactivateComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
		otherComponentType = shovelerComponentTypeCreate(otherComponentTypeId, /* activate */ NULL, updateComponent, /* deactivate */ NULL, /* requiresAuthority */ true, sizeof(otherConfigurationOptions) / sizeof(otherConfigurationOptions[0]), otherConfigurationOptions);
		yetAnotherComponentType = shovelerComponentTypeCreate(yetAnotherComponentTypeId, activateComponent, /* update */ NULL, deactivateComponent, /* requiresAuthority */ false, sizeof(yetAnotherConfigurationOptions) / sizeof(yetAnotherConfigurationOptions[0]), yetAnotherConfigurationOptions);

		component = shovelerComponentCreate(&viewAdapter, entityId, componentType);
		ASSERT_TRUE(component != NULL);

		otherComponent = shovelerComponentCreate(&viewAdapter, otherEntityId, otherComponentType);
		ASSERT_TRUE(otherComponent != NULL);

		yetAnotherComponent = shovelerComponentCreate(&viewAdapter, entityId, yetAnotherComponentType);
		ASSERT_TRUE(yetAnotherComponent != NULL);

		activateCalled = false;

		updateCalled = false;
		lastUpdateDt = 0.0;
		nextUpdateReturn = false;

		deactivateCalled = false;

		liveUpdateCalled = false;
		lastLiveUpdateConfigurationOption = NULL;
		lastLiveUpdateValue = NULL;
		nextLiveUpdateReturn = false;

		liveUpdateDependencyCalled = false;
		lastLiveUpdateDependencyConfigurationOption = NULL;
		lastLiveUpdateDependencyComponent = NULL;
		nextLiveUpdateDependencyReturn = false;
	}

	virtual void TearDown()
	{
		shovelerLogTrace("Tearing down test case.");
		shovelerComponentFree(yetAnotherComponent);
		shovelerComponentFree(otherComponent);
		shovelerComponentFree(component);
		shovelerComponentTypeFree(yetAnotherComponentType);
		shovelerComponentTypeFree(otherComponentType);
		shovelerComponentTypeFree(componentType);
	}

	const char *componentTypeId = "component type";
	const char *otherComponentTypeId = "other component type";
	const char *yetAnotherComponentTypeId = "yet another component type";

	const char *configurationOptionName = "option";
	const char *dependencyConfigurationOptionName = "dependency";
	const char *liveUpdateConfigurationOptionName = "live update";

	const long long int entityId = 1;
	const long long int otherEntityId = 2;

	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	ShovelerComponentTypeConfigurationOption otherConfigurationOptions[1];
	ShovelerComponentTypeConfigurationOption yetAnotherConfigurationOptions[1];

	ShovelerComponentViewAdapter viewAdapter;
	ShovelerComponentType *componentType;
	ShovelerComponentType *otherComponentType;
	ShovelerComponentType *yetAnotherComponentType;
	ShovelerComponent *component;
	ShovelerComponent *otherComponent;
	ShovelerComponent *yetAnotherComponent;

	std::map<std::pair<long long int, std::string>, std::set<ShovelerComponent *> > reverseDependencies;

	bool activateCalled;

	bool updateCalled;
	double lastUpdateDt;
	bool nextUpdateReturn;

	bool deactivateCalled;

	bool liveUpdateCalled;
	const ShovelerComponentTypeConfigurationOption *lastLiveUpdateConfigurationOption;
	const ShovelerComponentConfigurationValue *lastLiveUpdateValue;
	bool nextLiveUpdateReturn;

	bool liveUpdateDependencyCalled;
	const ShovelerComponentTypeConfigurationOption *lastLiveUpdateDependencyConfigurationOption;
	ShovelerComponent *lastLiveUpdateDependencyComponent;
	bool nextLiveUpdateDependencyReturn;
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
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);

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

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	ASSERT_TRUE(deactivateCalled) << "test component should be deactivated after adding unsatisfied dependency";
	ASSERT_FALSE(shovelerComponentIsActive(component));
}

TEST_F(ShovelerComponentTest, deactivateReverseDependencies)
{
	shovelerComponentActivate(component);
	shovelerComponentActivate(otherComponent);

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);

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

	int firstValue = shovelerComponentGetConfigurationValueInt(component, COMPONENT_CONFIGURATION_OPTION);
	ASSERT_EQ(firstValue, 0);

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionInt(component, COMPONENT_CONFIGURATION_OPTION, newConfigurationValue);
	ASSERT_TRUE(updated);

	int secondValue = shovelerComponentGetConfigurationValueInt(component, COMPONENT_CONFIGURATION_OPTION);
	ASSERT_EQ(secondValue, newConfigurationValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationWithReactivation) {
	const int newConfigurationValue = 27;

	shovelerComponentActivate(component);
	activateCalled = false;

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionInt(component, COMPONENT_CONFIGURATION_OPTION, newConfigurationValue);
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

	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(otherComponent, OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(liveUpdateCalled);
	ASSERT_EQ(lastLiveUpdateConfigurationOption->name, liveUpdateConfigurationOptionName);
	ASSERT_STREQ(lastLiveUpdateValue->stringValue, newConfigurationValue);
	ASSERT_FALSE(activateCalled);
	ASSERT_FALSE(deactivateCalled);

	const char *newValue = shovelerComponentGetConfigurationValueString(otherComponent, OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE);
	ASSERT_STREQ(newValue, newConfigurationValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveUpdatesReverseDependency) {
	const char *newConfigurationValue = "new value";

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	nextLiveUpdateReturn = true;
	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(otherComponent, OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(liveUpdateDependencyCalled);
	ASSERT_EQ(lastLiveUpdateDependencyConfigurationOption->name, dependencyConfigurationOptionName);
	ASSERT_EQ(lastLiveUpdateDependencyComponent, otherComponent);
	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(activateCalled);
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveWithoutPropagation) {
	const char *newConfigurationValue = "new value";

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	nextLiveUpdateReturn = false;
	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(otherComponent, OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_FALSE(liveUpdateDependencyCalled);
	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(activateCalled);
}

TEST_F(ShovelerComponentTest, updateComponentUpdatesReverseDependency) {
	double dt = 1234.5;
	const char *newConfigurationValue = "new value";

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	nextUpdateReturn = true;
	bool updated = shovelerComponentUpdate(otherComponent, dt);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateCalled);
	ASSERT_EQ(lastUpdateDt, dt);
	ASSERT_TRUE(liveUpdateDependencyCalled);
	ASSERT_EQ(lastLiveUpdateDependencyConfigurationOption->name, dependencyConfigurationOptionName);
	ASSERT_EQ(lastLiveUpdateDependencyComponent, otherComponent);
	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(activateCalled);
}

TEST_F(ShovelerComponentTest, updateConfigurationLiveReactivatesReverseDependency) {
	const char *newConfigurationValue = "new value";

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_REACTIVATE_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	nextLiveUpdateReturn = true;
	bool updated = shovelerComponentUpdateCanonicalConfigurationOptionString(otherComponent, OTHER_COMPONENT_CONFIGURATION_OPTION_LIVE_UPDATE, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_FALSE(liveUpdateDependencyCalled);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(activateCalled);
}

TEST_F(ShovelerComponentTest, updateComponentReactivatesReverseDependency) {
	double dt = 1234.5;

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_REACTIVATE_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	nextUpdateReturn = true;
	bool updated = shovelerComponentUpdate(otherComponent, dt);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateCalled);
	ASSERT_EQ(lastUpdateDt, dt);
	ASSERT_FALSE(liveUpdateDependencyCalled);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(activateCalled);
}

TEST_F(ShovelerComponentTest, nonPropagatingUpdateComponentDoesntAffectReverseDependency) {
	double dt = 1234.5;

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_REACTIVATE_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	nextUpdateReturn = false;
	bool updated = shovelerComponentUpdate(otherComponent, dt);
	ASSERT_FALSE(updated);
	ASSERT_TRUE(updateCalled);
	ASSERT_EQ(lastUpdateDt, dt);
	ASSERT_FALSE(liveUpdateDependencyCalled);
	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(activateCalled);
}

TEST_F(ShovelerComponentTest, doublePropagateUpdate) {
	double dt = 1234.5;

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(yetAnotherComponent, YET_ANOTHER_COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, entityId);
	bool secondDependencyActivated = shovelerComponentActivate(yetAnotherComponent);
	ASSERT_TRUE(secondDependencyActivated);
	activateCalled = false;

	nextUpdateReturn = true;
	nextLiveUpdateDependencyReturn = true;
	bool updated = shovelerComponentUpdate(otherComponent, dt);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateCalled);
	ASSERT_EQ(lastUpdateDt, dt);
	ASSERT_TRUE(liveUpdateDependencyCalled);
	ASSERT_EQ(lastLiveUpdateDependencyConfigurationOption->name, dependencyConfigurationOptionName);
	ASSERT_EQ(lastLiveUpdateDependencyComponent, otherComponent);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(activateCalled);
}

TEST_F(ShovelerComponentTest, dontPropagateLiveDependencyUpdate) {
	double dt = 1234.5;

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, otherEntityId);
	shovelerComponentDelegate(otherComponent);
	bool dependencyActivated = shovelerComponentActivate(otherComponent);
	ASSERT_TRUE(dependencyActivated);
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	activateCalled = false;

	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(yetAnotherComponent, YET_ANOTHER_COMPONENT_CONFIGURATION_OPTION_DEPENDENCY, entityId);
	bool secondDependencyActivated = shovelerComponentActivate(yetAnotherComponent);
	ASSERT_TRUE(secondDependencyActivated);
	activateCalled = false;

	nextUpdateReturn = true;
	nextLiveUpdateDependencyReturn = false;
	bool updated = shovelerComponentUpdate(otherComponent, dt);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateCalled);
	ASSERT_EQ(lastUpdateDt, dt);
	ASSERT_TRUE(liveUpdateDependencyCalled);
	ASSERT_EQ(lastLiveUpdateDependencyConfigurationOption->name, dependencyConfigurationOptionName);
	ASSERT_EQ(lastLiveUpdateDependencyComponent, otherComponent);
	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(activateCalled);
}

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentTypeId, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	if(entityId == test->entityId && strcmp(componentTypeId, test->componentTypeId) == 0) {
		return test->component;
	}

	if(entityId == test->otherEntityId && strcmp(componentTypeId, test->otherComponentTypeId) == 0) {
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

static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	auto dependencyTarget = std::make_pair(targetEntityId, targetComponentTypeId);
	test->reverseDependencies[dependencyTarget].insert(component);
}

static bool removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	auto dependencyTarget = std::make_pair(targetEntityId, targetComponentTypeId);
	test->reverseDependencies[dependencyTarget].erase(component);
	return true;
}

static void forEachReverseDependency(ShovelerComponent *component, ShovelerComponentAdapterViewForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;

	auto dependencyTarget = std::make_pair(component->entityId, component->type->id);
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

static bool updateComponent(ShovelerComponent *component, double dt)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->updateCalled = true;
	test->lastUpdateDt = dt;
	return test->nextUpdateReturn;
}

static void deactivateComponent(ShovelerComponent *component)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->deactivateCalled = true;
}

static bool liveUpdateComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->liveUpdateCalled = true;
	test->lastLiveUpdateConfigurationOption = configurationOption;
	test->lastLiveUpdateValue = value;

	return test->nextLiveUpdateReturn;
}

static bool liveUpdateDependencyComponent(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) shovelerComponentGetViewTarget(component, testTargetName);
	test->liveUpdateDependencyCalled = true;
	test->lastLiveUpdateDependencyConfigurationOption = configurationOption;
	test->lastLiveUpdateDependencyComponent = dependencyComponent;

	return test->nextLiveUpdateDependencyReturn;
}
