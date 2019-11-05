#include <cstring> // memcmp
#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/view/resource.h"
#include "shoveler/view/resources.h"
#include "shoveler/component.h"
#include "shoveler/resources.h"
}

static void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, int bufferSize);

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

		view = shovelerViewCreate(NULL, NULL);
		bool resourcesTargetAdded = shovelerViewSetResources(view, resources);
		ASSERT_TRUE(resourcesTargetAdded);

		lastLoadBuffer = NULL;
		lastLoadBufferSize = 0;
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
	int lastLoadBufferSize;
	void *nextLoadResourceData;
};

TEST_F(ShovelerViewResourcesTest, addResource)
{
	long long int testEntityId = 42;
	const char *testResourceId = "42";
	unsigned char testResourceBuffer = 27;
	int testResourceBytes = 1;
	const char *testResourceData = "test resource data";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	nextLoadResourceData = (void *) testResourceData;
	const ShovelerViewResourceConfiguration configuration{testTypeId, &testResourceBuffer, testResourceBytes};
	bool resourceComponentAdded = shovelerViewEntityAddResource(testEntity, &configuration);
	ASSERT_TRUE(resourceComponentAdded);
	ASSERT_EQ(lastLoadBufferSize, testResourceBytes) << "load should be called with correct bytes";
	ASSERT_EQ(memcmp(&testResourceBuffer, lastLoadBuffer, testResourceBytes), 0) << "load should be called with correct buffer";

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	ASSERT_TRUE(resource != NULL);
	ASSERT_TRUE(resource->data == testResourceData);
}

TEST_F(ShovelerViewResourcesTest, updateResource)
{
	long long int testEntityId = 42;
	const char *testResourceId = "42";
	unsigned char testResourceBuffer = 27;
	int testResourceBytes = 1337;
	const char *testResourceData = "test resource data";
	const char *otherTestResourceData = "other test resource data";

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	nextLoadResourceData = (void *) testResourceData;
	const ShovelerViewResourceConfiguration configuration{testTypeId, &testResourceBuffer, testResourceBytes};
	ShovelerComponent *testComponent = shovelerViewEntityAddResource(testEntity, &configuration);
	ASSERT_TRUE(testComponent != NULL);

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	nextLoadResourceData = (void *) otherTestResourceData;

	bool resourceComponentUpdated = shovelerComponentUpdateCanonicalConfigurationOptionString(testComponent, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_TYPE_ID, testTypeId);
	ASSERT_TRUE(resourceComponentUpdated);
	ASSERT_TRUE(resource->data == otherTestResourceData) << "resource data should have changed after component update";
}

TEST_F(ShovelerViewResourcesTest, removeResource)
{
	long long int testEntityId = 42;
	const char *testResourceId = "42";
	unsigned char testResourceBuffer = 27;
	int testResourceBytes = 1337;

	bool entityAdded = shovelerViewAddEntity(view, testEntityId);
	ASSERT_TRUE(entityAdded);

	ShovelerViewEntity *testEntity = shovelerViewGetEntity(view, testEntityId);
	ASSERT_TRUE(testEntity != NULL);

	const ShovelerViewResourceConfiguration configuration{testTypeId, &testResourceBuffer, testResourceBytes};
	bool resourceComponentAdded = shovelerViewEntityAddResource(testEntity, &configuration);
	ASSERT_TRUE(resourceComponentAdded);
	bool resourceComponentRemoved = shovelerViewEntityRemoveResource(testEntity);
	ASSERT_TRUE(resourceComponentRemoved);

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	ASSERT_TRUE(resource != NULL) << "resource should still be available despite component removal";
}

static void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, int bufferSize)
{
	ShovelerViewResourcesTest *test = (ShovelerViewResourcesTest *) typeLoader->data;
	test->lastLoadBuffer = buffer;
	test->lastLoadBufferSize = bufferSize;

	return test->nextLoadResourceData;
}
