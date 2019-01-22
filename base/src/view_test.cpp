#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/view.h"
}

static bool activateComponent(ShovelerViewComponent *component, void *testPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *testPointer);
static void freeComponent(ShovelerViewComponent *component, void *testPointer);
static void dependencyCallbackFunction(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencySource, const ShovelerViewQualifiedComponent *dependencyTarget, bool added, void *testPointer);

class ShovelerViewTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		view = shovelerViewCreate();
		activateCalled = false;
		deactivateCalled = false;
		freeCalled = false;
		lastDependencySource.entityId = 0;
		lastDependencySource.componentName = NULL;
		lastDependencyTarget.entityId = 0;
		lastDependencyTarget.componentName = NULL;
		lastDependencyAdded = false;
	}

	virtual void TearDown()
	{
		clearDependencyCallback();
		shovelerViewFree(view);
	}

	void clearDependencyCallback()
	{
		free(lastDependencySource.componentName);
		free(lastDependencyTarget.componentName);

		lastDependencySource.entityId = 0;
		lastDependencySource.componentName = NULL;
		lastDependencyTarget.entityId = 0;
		lastDependencyTarget.componentName = NULL;
		lastDependencyAdded = false;
	}

	ShovelerView *view;
	bool activateCalled;
	bool deactivateCalled;
	bool freeCalled;
	ShovelerViewQualifiedComponent lastDependencySource;
	ShovelerViewQualifiedComponent lastDependencyTarget;
	bool lastDependencyAdded;
};

TEST_F(ShovelerViewTest, activate)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";
	long long int testDependencyEntityId = 42;
	const char *testDependencyComponentName = "dependency";

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerViewComponent *testComponent = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(testComponent != NULL);

	shovelerViewComponentAddDependency(testComponent, testDependencyEntityId, testDependencyComponentName);

	bool activated = shovelerViewComponentActivate(testComponent);
	ASSERT_FALSE(activated);

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);

	ShovelerViewComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL, NULL, NULL);
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

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerViewComponent *testComponent = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
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

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerViewComponent *testComponent = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(testComponent != NULL);

	ShovelerViewEntity *testDependencyEntity = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(testDependencyEntity != NULL);

	ShovelerViewComponent *testDependencyComponent = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL, NULL, NULL);
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

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerViewComponent *testComponent = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
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

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerViewComponent *testComponent = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
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

TEST_F(ShovelerViewTest, dependencyCallbacks)
{
	long long int testEntityId = 1337;
	const char *testComponentName = "test";
	long long int testDependencyEntityId = 42;
	const char *testDependencyComponentName = "dependency";

	shovelerViewAddDependencyCallback(view, dependencyCallbackFunction, this);

	ShovelerViewEntity *testEntity = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	ShovelerViewComponent *testComponent = shovelerViewEntityAddComponent(testEntity, testComponentName, this, activateComponent, deactivateComponent, freeComponent);
	ASSERT_TRUE(testComponent != NULL);

	shovelerViewComponentAddDependency(testComponent, testDependencyEntityId, testDependencyComponentName);

	ASSERT_EQ(lastDependencySource.entityId, testEntityId);
	ASSERT_STREQ(lastDependencySource.componentName, testComponentName);
	ASSERT_EQ(lastDependencyTarget.entityId, testDependencyEntityId);
	ASSERT_STREQ(lastDependencyTarget.componentName, testDependencyComponentName);
	ASSERT_TRUE(lastDependencyAdded);

	clearDependencyCallback();
	shovelerViewComponentRemoveDependency(testComponent, testDependencyEntityId, testDependencyComponentName);

	ASSERT_EQ(lastDependencySource.entityId, testEntityId);
	ASSERT_STREQ(lastDependencySource.componentName, testComponentName);
	ASSERT_EQ(lastDependencyTarget.entityId, testDependencyEntityId);
	ASSERT_STREQ(lastDependencyTarget.componentName, testDependencyComponentName);
	ASSERT_FALSE(lastDependencyAdded);
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

static void dependencyCallbackFunction(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencySource, const ShovelerViewQualifiedComponent *dependencyTarget, bool added, void *testPointer)
{
	ShovelerViewTest *test = (ShovelerViewTest *) testPointer;
	test->lastDependencySource.entityId = dependencySource->entityId;
	test->lastDependencySource.componentName = strdup(dependencySource->componentName);
	test->lastDependencyTarget.entityId = dependencyTarget->entityId;
	test->lastDependencyTarget.componentName = strdup(dependencyTarget->componentName);
	test->lastDependencyAdded = added;
}
