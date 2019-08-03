#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
}

static void *activateComponent(ShovelerComponent *component, void *testPointer);
static void deactivateComponent(ShovelerComponent *component, void *testPointer);
static void updateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *testPointer);

class ShovelerComponentTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		componentType = shovelerComponentTypeCreate(componentTypeName, activateComponent, deactivateComponent);
		shovelerComponentTypeAddConfigurationOptionInt(componentType, configurationOptionKey, configurationOptionDefaultValue, updateComponent);
		shovelerComponentTypeAddConfigurationOptionString(componentType, otherConfigurationOptionKey, otherConfigurationOptionDefaultValue, NULL);

		component = shovelerComponentCreate(componentType, this);

		activateCalled = false;
		deactivateCalled = false;
		updateCalled = false;
		lastUpdateConfigurationOption = NULL;
	}

	virtual void TearDown()
	{
		shovelerComponentFree(component);
		shovelerComponentTypeFree(componentType);
	}

	const char *componentTypeName = "component type";
	const char *configurationOptionKey = "configuration option key";
	const char *otherConfigurationOptionKey = "other configuration option key";
	const int configurationOptionDefaultValue = 42;
	const char *otherConfigurationOptionDefaultValue = "default value";

	ShovelerComponentType *componentType;
	ShovelerComponent *component;

	bool activateCalled;
	bool deactivateCalled;
	bool updateCalled;
	ShovelerComponentTypeConfigurationOption *lastUpdateConfigurationOption;
	const ShovelerComponentConfigurationValue *lastUpdateValue;
};

TEST_F(ShovelerComponentTest, activate) {
	bool activated = shovelerComponentActivate(component);
	ASSERT_TRUE(activated);
	ASSERT_TRUE(activateCalled);
	ASSERT_EQ(component->data, this);

	activateCalled = false;
	bool activatedAgain = shovelerComponentActivate(component);
	ASSERT_FALSE(activatedAgain);
	ASSERT_FALSE(activateCalled);
}

TEST_F(ShovelerComponentTest, deactivate) {
	shovelerComponentActivate(component);
	bool deactivated = shovelerComponentDeactivate(component);
	ASSERT_TRUE(deactivated);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(component->data == NULL);

	deactivateCalled = false;
	bool deactivatedAgain = shovelerComponentDeactivate(component);
	ASSERT_FALSE(deactivatedAgain);
	ASSERT_FALSE(deactivateCalled);
}

TEST_F(ShovelerComponentTest, updateConfiguration) {
	const int newConfigurationValue = 27;

	int firstValue = shovelerComponentGetConfigurationValueInt(component, configurationOptionKey);
	ASSERT_EQ(firstValue, configurationOptionDefaultValue);

	bool updated = shovelerComponentUpdateConfigurationOptionInt(component, configurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);

	int secondValue = shovelerComponentGetConfigurationValueInt(component, configurationOptionKey);
	ASSERT_EQ(secondValue, newConfigurationValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationLive) {
	const int newConfigurationValue = 27;

	shovelerComponentActivate(component);
	activateCalled = false;

	bool updated = shovelerComponentUpdateConfigurationOptionInt(component, configurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_TRUE(updateCalled);
	ASSERT_STREQ(lastUpdateConfigurationOption->key, configurationOptionKey);
	ASSERT_EQ(lastUpdateValue->intValue, newConfigurationValue);
	ASSERT_FALSE(activateCalled);
	ASSERT_FALSE(deactivateCalled);

	int newValue = shovelerComponentGetConfigurationValueInt(component, configurationOptionKey);
	ASSERT_EQ(newValue, newConfigurationValue);
}

TEST_F(ShovelerComponentTest, updateConfigurationWithReactivation) {
	const char *newConfigurationValue = "new value";

	shovelerComponentActivate(component);
	activateCalled = false;

	bool updated = shovelerComponentUpdateConfigurationOptionString(component, otherConfigurationOptionKey, newConfigurationValue);
	ASSERT_TRUE(updated);
	ASSERT_FALSE(updateCalled);
	ASSERT_TRUE(activateCalled);
	ASSERT_TRUE(deactivateCalled);

	const char *newValue = shovelerComponentGetConfigurationValueString(component, otherConfigurationOptionKey);
	ASSERT_STREQ(newValue, newConfigurationValue);
}

static void *activateComponent(ShovelerComponent *component, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;
	test->activateCalled = true;
	return test;
}

static void deactivateComponent(ShovelerComponent *component, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;
	test->deactivateCalled = true;
}

static void updateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;
	test->updateCalled = true;
	test->lastUpdateConfigurationOption = configurationOption;
	test->lastUpdateValue = value;
}
