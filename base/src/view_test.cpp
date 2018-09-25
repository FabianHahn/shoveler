#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/view.h"
}

static void activateComponent(void *testPointer);
static void deactivateComponent(void *testPointer);
static void freeComponent(ShovelerViewComponent *component);

class ShovelerViewTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		view = shovelerViewCreate();
		activateCalled = false;
		deactivateCalled = false;
		freeCalled = false;
	}

	virtual void TearDown()
	{
		shovelerViewFree(view);
	}

	ShovelerView *view;
	bool activateCalled;
	bool deactivateCalled;
	bool freeCalled;
};

TEST_F(ShovelerViewTest, activate)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";
	long long int testDependencyEntityId = 42;
	const char *testDependencyComponentName = "dependency";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(componentAdded);

	bool dependencyAdded = shovelerViewEntityAddComponentDependency(testEntity, testComponentName, testDependencyEntityId, testDependencyComponentName);
	ASSERT_TRUE(dependencyAdded);

	bool activated = shovelerViewEntityActivateComponent(testEntity, testComponentName);
	ASSERT_FALSE(activated);

	bool dependencyEntityAdded = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(dependencyEntityAdded);

	ShovelerViewEntity *testDependencyEntity = shovelerViewGetEntity(view, testDependencyEntityId);
	bool dependencyComponentAdded = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL, NULL, NULL);
	ASSERT_TRUE(dependencyComponentAdded);

	ASSERT_FALSE(activateCalled);
	bool dependencyActivated = shovelerViewEntityActivateComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(dependencyActivated);
	ASSERT_TRUE(activateCalled) << "test component has also been activated after activating its dependency";
}

TEST_F(ShovelerViewTest, deactivateWhenAddingUnsatisfiedDependency)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";
	long long int testDependencyEntityId = 42;
	const char *testDependencyComponentName = "dependency";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(componentAdded);

	bool activated = shovelerViewEntityActivateComponent(testEntity, testComponentName);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	bool dependencyAdded = shovelerViewEntityAddComponentDependency(testEntity, testComponentName, testDependencyEntityId, testDependencyComponentName);
	ASSERT_TRUE(dependencyAdded);
	ASSERT_TRUE(deactivateCalled) << "test component should be deactivated after adding unsatisfied dependency";
}

TEST_F(ShovelerViewTest, deactivateReverseDependencies)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";
	long long int testDependencyEntityId = 42;
	const char *testDependencyComponentName = "dependency";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(componentAdded);

	bool dependencyEntityAdded = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(dependencyEntityAdded);

	ShovelerViewEntity *testDependencyEntity = shovelerViewGetEntity(view, testDependencyEntityId);
	bool dependencyComponentAdded = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL, NULL, NULL);
	ASSERT_TRUE(dependencyComponentAdded);

	bool dependencyAdded = shovelerViewEntityAddComponentDependency(testEntity, testComponentName, testDependencyEntityId, testDependencyComponentName);
	ASSERT_TRUE(dependencyAdded);

	bool activated = shovelerViewEntityActivateComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	bool deactivated = shovelerViewEntityDeactivateComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(deactivated);
	ASSERT_TRUE(deactivateCalled) << "test component has also been deactivated after deactivating its dependency";
}

TEST_F(ShovelerViewTest, deactivateRemovedComponent)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(componentAdded);

	bool activated = shovelerViewEntityActivateComponent(testEntity, testComponentName);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(freeCalled);
	bool removed = shovelerViewEntityRemoveComponent(testEntity, testComponentName);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(freeCalled);
}

TEST_F(ShovelerViewTest, removeEntityRemovesComponents)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(componentAdded);

	bool activated = shovelerViewEntityActivateComponent(testEntity, testComponentName);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(freeCalled);
	bool removed = shovelerViewRemoveEntity(view, testEntityId);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(freeCalled);
}

static void activateComponent(void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) testPointer;
	test->activateCalled = true;
}

static void deactivateComponent(void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) testPointer;
	test->deactivateCalled = true;
}

static void freeComponent(ShovelerViewComponent *component)
{
	ShovelerViewTest *test = (ShovelerViewTest *) component->data;
	test->freeCalled = true;
}