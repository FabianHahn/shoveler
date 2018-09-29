#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/view.h"
}

static bool activateComponent(ShovelerViewComponent *component, void *testPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *testPointer);
static void freeComponent(ShovelerViewComponent *component, void *testPointer);

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

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_TRUE(testComponent != NULL);

	shovelerViewComponentAddDependency(testComponent, testDependencyEntityId, testDependencyComponentName);

	bool activated = shovelerViewComponentActivate(testComponent);
	ASSERT_FALSE(activated);

	bool dependencyEntityAdded = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(dependencyEntityAdded);

	ShovelerViewEntity *testDependencyEntity = shovelerViewGetEntity(view, testDependencyEntityId);
	bool dependencyComponentAdded = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL, NULL, NULL);
	ASSERT_TRUE(dependencyComponentAdded);

	ShovelerViewComponent *testDependencyComponent = shovelerViewEntityGetComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(testDependencyComponent != NULL);

	ASSERT_FALSE(activateCalled);
	bool dependencyActivated = shovelerViewComponentActivate(testDependencyComponent);
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

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_TRUE(testComponent != NULL);

	bool activated = shovelerViewComponentActivate(testComponent);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	shovelerViewComponentAddDependency(testComponent, testDependencyEntityId, testDependencyComponentName);
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

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_TRUE(testComponent != NULL);

	bool dependencyEntityAdded = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(dependencyEntityAdded);

	ShovelerViewEntity *testDependencyEntity = shovelerViewGetEntity(view, testDependencyEntityId);
	bool dependencyComponentAdded = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL, NULL, NULL);
	ASSERT_TRUE(dependencyComponentAdded);

	ShovelerViewComponent *testDependencyComponent = shovelerViewEntityGetComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(testDependencyComponent != NULL);

	shovelerViewComponentAddDependency(testComponent, testDependencyEntityId, testDependencyComponentName);

	bool activated = shovelerViewComponentActivate(testDependencyComponent);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	shovelerViewComponentDeactivate(testDependencyComponent);
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

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_TRUE(testComponent != NULL);

	bool activated = shovelerViewComponentActivate(testComponent);
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

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_TRUE(testComponent != NULL);

	bool activated = shovelerViewComponentActivate(testComponent);
	ASSERT_TRUE(activated);

	ASSERT_FALSE(deactivateCalled);
	ASSERT_FALSE(freeCalled);
	bool removed = shovelerViewRemoveEntity(view, testEntityId);
	ASSERT_TRUE(removed);
	ASSERT_TRUE(deactivateCalled);
	ASSERT_TRUE(freeCalled);
}

static bool activateComponent(ShovelerViewComponent *component, void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) testPointer;
	test->activateCalled = true;
	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) testPointer;
	test->deactivateCalled = true;
}

static void freeComponent(ShovelerViewComponent *component, void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) component->data;
	test->freeCalled = true;
}
