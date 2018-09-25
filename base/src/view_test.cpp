#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/view.h"
}

class ShovelerViewTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		view = shovelerViewCreate();
	}

	virtual void TearDown()
	{
		shovelerViewFree(view);
	}

	ShovelerView *view;
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
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, NULL, NULL);
	ASSERT_TRUE(componentAdded);

	bool dependencyAdded = shovelerViewEntityAddComponentDependency(testEntity, testComponentName, testDependencyEntityId, testDependencyComponentName);
	ASSERT_TRUE(dependencyAdded);

	bool activated = shovelerViewEntityActivateComponent(testEntity, testComponentName);
	ASSERT_FALSE(activated);

	bool dependencyEntityAdded = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(dependencyEntityAdded);

	ShovelerViewEntity *testDependencyEntity = shovelerViewGetEntity(view, testDependencyEntityId);
	bool dependencyComponentAdded = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL);
	ASSERT_TRUE(dependencyComponentAdded);

	bool dependencyActivated = shovelerViewEntityActivateComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(dependencyActivated);

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_TRUE(testComponent->active) << "test component has also been activated after activating its reverse dependency";
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
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, NULL, NULL);
	ASSERT_TRUE(componentAdded);

	bool activated = shovelerViewEntityActivateComponent(testEntity, testComponentName);
	ASSERT_TRUE(activated);

	bool dependencyAdded = shovelerViewEntityAddComponentDependency(testEntity, testComponentName, testDependencyEntityId, testDependencyComponentName);
	ASSERT_TRUE(dependencyAdded);

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_FALSE(testComponent->active) << "test component should be deactivated after adding unsatisfied dependency";
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
	bool componentAdded = shovelerViewEntityAddComponent(testEntity, testComponentName, NULL, NULL);
	ASSERT_TRUE(componentAdded);

	bool dependencyEntityAdded = shovelerViewAddEntity(view, testDependencyEntityId);
	ASSERT_TRUE(dependencyEntityAdded);

	ShovelerViewEntity *testDependencyEntity = shovelerViewGetEntity(view, testDependencyEntityId);
	bool dependencyComponentAdded = shovelerViewEntityAddComponent(testDependencyEntity, testDependencyComponentName, NULL, NULL);
	ASSERT_TRUE(dependencyComponentAdded);

	bool dependencyAdded = shovelerViewEntityAddComponentDependency(testEntity, testComponentName, testDependencyEntityId, testDependencyComponentName);
	ASSERT_TRUE(dependencyAdded);

	bool activated = shovelerViewEntityActivateComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(activated);

	bool deactivated = shovelerViewEntityDeactivateComponent(testDependencyEntity, testDependencyComponentName);
	ASSERT_TRUE(deactivated);

	ShovelerViewComponent *testComponent = shovelerViewEntityGetComponent(testEntity, testComponentName);
	ASSERT_FALSE(testComponent->active) << "test component has also been deactivated after deactivating its dependency";
}
