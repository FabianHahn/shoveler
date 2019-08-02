#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
}

static void *activateComponent(ShovelerComponent *component, void *testPointer);
static void deactivateComponent(ShovelerComponent *component, void *testPointer);
static void updateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponentConfigurationValue previousValue, void *testPointer);

class ShovelerComponentTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		componentType = shovelerComponentTypeCreate(componentTypeName, activateComponent, deactivateComponent, this);
		component = shovelerComponentCreate(componentType);

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

	ShovelerComponentType *componentType;
	ShovelerComponent *component;

	bool activateCalled;
	bool deactivateCalled;
	bool updateCalled;
	ShovelerComponentTypeConfigurationOption *lastUpdateConfigurationOption;
	ShovelerComponentConfigurationValue lastUpdatePreviousValue;
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

static void updateComponent(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponentConfigurationValue previousValue, void *testPointer)
{
	ShovelerComponentTest *test = (ShovelerComponentTest *) testPointer;
	test->updateCalled = true;
	test->lastUpdateConfigurationOption = configurationOption;
	test->lastUpdatePreviousValue = previousValue;
}
