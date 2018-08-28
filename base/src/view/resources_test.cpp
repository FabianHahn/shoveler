#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/view/resources.h"
}

static void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, size_t bytes);

class ShovelerViewResourcesTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		resources = shovelerResourcesCreate(NULL, NULL);

		testTypeId = "test type id";
		testDefaultResourceData = "test default resource";

		testTypeLoader.typeId = testTypeId;
		testTypeLoader.defaultResourceData = (void *) testDefaultResourceData;
		testTypeLoader.data = this;
		testTypeLoader.load = loadResource;
		testTypeLoader.freeResourceData = NULL;
		testTypeLoader.free = NULL;
		bool typeLoaderRegistered = shovelerResourcesRegisterTypeLoader(resources, testTypeLoader);
		ASSERT_TRUE(typeLoaderRegistered) << "test type loader should register correctly";

		view = shovelerViewCreate();
		bool resourcesTargetAdded = shovelerViewSetResources(view, resources);
		ASSERT_TRUE(resourcesTargetAdded);

		lastLoadBuffer = NULL;
		lastLoadBytes = 0;
		nextLoadResourceData = NULL;
	}

	virtual void TearDown()
	{
		shovelerViewFree(view);
		shovelerResourcesFree(resources);
	}

	ShovelerResources *resources;
	const char *testTypeId;
	const char *testDefaultResourceData;
	ShovelerResourcesTypeLoader testTypeLoader;
	ShovelerView *view;

	const unsigned char *lastLoadBuffer;
	size_t lastLoadBytes;
	void *nextLoadResourceData;
};

TEST_F(ShovelerViewResourcesTest, addResource)
{
	long long int testEntityId = 42;
	const char *testResourceId = "42";
	unsigned char testResourceBuffer = 27;
	size_t testResourceBytes = 1337;
	const char *testResourceData = "test resource data";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	nextLoadResourceData = (void *) testResourceData;
	bool resourceComponentAdded = shovelerViewAddEntityResource(view, testEntityId, testTypeId, &testResourceBuffer, testResourceBytes);
	ASSERT_TRUE(resourceComponentAdded);
	ASSERT_EQ(lastLoadBuffer, &testResourceBuffer) << "load should be called with correct buffer";
	ASSERT_EQ(lastLoadBytes, testResourceBytes) << "load should be called with correct bytes";

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	ASSERT_TRUE(resource != NULL);
	ASSERT_TRUE(resource->data == testResourceData);
}

TEST_F(ShovelerViewResourcesTest, updateResource)
{
	long long int testEntityId = 42;
	const char *testResourceId = "42";
	unsigned char testResourceBuffer = 27;
	size_t testResourceBytes = 1337;
	const char *testResourceData = "test resource data";
	const char *otherTestResourceData = "other test resource data";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	nextLoadResourceData = (void *) testResourceData;
	bool resourceComponentAdded = shovelerViewAddEntityResource(view, testEntityId, testTypeId, &testResourceBuffer, testResourceBytes);
	ASSERT_TRUE(resourceComponentAdded);

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	nextLoadResourceData = (void *) otherTestResourceData;
	bool resourceComponentUpdated = shovelerViewUpdateEntityResource(view, testEntityId, &testResourceBuffer, testResourceBytes);
	ASSERT_TRUE(resourceComponentUpdated);
	ASSERT_TRUE(resource->data == otherTestResourceData) << "resource data should have changed after component update";
}

TEST_F(ShovelerViewResourcesTest, removeResource)
{
	long long int testEntityId = 42;
	const char *testResourceId = "42";
	unsigned char testResourceBuffer = 27;
	size_t testResourceBytes = 1337;

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	bool resourceComponentAdded = shovelerViewAddEntityResource(view, testEntityId, testTypeId, &testResourceBuffer, testResourceBytes);
	ASSERT_TRUE(resourceComponentAdded);
	bool resourceComponentRemoved = shovelerViewRemoveEntityResource(view, testEntityId);
	ASSERT_TRUE(resourceComponentRemoved);

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	ASSERT_TRUE(resource != NULL) << "resource should still be available despite component removal";
}

static void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, size_t bytes)
{
	ShovelerViewResourcesTest *test = (ShovelerViewResourcesTest *) typeLoader->data;
	test->lastLoadBuffer = buffer;
	test->lastLoadBytes = bytes;

	return test->nextLoadResourceData;
}
