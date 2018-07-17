#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/resources.h"
}

static void requestResources(ShovelerResources *resources, const char *typeId, const char *resourceId, void *testPointer);
void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, size_t bytes);
void freeResource(ShovelerResourcesTypeLoader *typeLoader, void *resourceData);
void freeTypeLoader(ShovelerResourcesTypeLoader *typeLoader);

class ShovelerResourcesTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		resources = shovelerResourcesCreate(requestResources, this);

		testTypeId = "test type id";
		testDefaultResourceData = "test default resource";

		testTypeLoader.typeId = testTypeId;
		testTypeLoader.defaultResource = (void *) testDefaultResourceData;
		testTypeLoader.data = this;
		testTypeLoader.load = loadResource;
		testTypeLoader.freeResourceData = freeResource;
		testTypeLoader.free = freeTypeLoader;
		bool typeLoaderRegistered = shovelerResourcesRegisterTypeLoader(resources, testTypeLoader);
		ASSERT_TRUE(typeLoaderRegistered) << "test type loader should register correctly";

		lastRequestResources = NULL;
		lastRequestTypeId = NULL;
		lastRequestResourceId = NULL;

		lastLoadBuffer = NULL;
		lastLoadBytes = 0;
		nextLoadResourceData = NULL;

		lastFreeResourceData = NULL;

		freeTypeLoaderCalled = false;
	}

	virtual void TearDown()
	{
		shovelerResourcesFree(resources);

		ASSERT_TRUE(freeTypeLoaderCalled) << "type loader should be freed after resources was freed";
	}

	ShovelerResources *resources;
	const char *testTypeId;
	const char *testDefaultResourceData;
	ShovelerResourcesTypeLoader testTypeLoader;

	ShovelerResources *lastRequestResources;
	const char *lastRequestTypeId;
	const char *lastRequestResourceId;

	const unsigned char *lastLoadBuffer;
	size_t lastLoadBytes;
	void *nextLoadResourceData;

	void *lastFreeResourceData;

	bool freeTypeLoaderCalled;
};

TEST_F(ShovelerResourcesTest, requestUnloaded)
{
	const char *testResourceId = "test resource id";

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	ASSERT_EQ(lastRequestResources, resources) << "request should be called with correct resources";
	ASSERT_EQ(lastRequestTypeId, testTypeId) << "request should be called with correct type id";
	ASSERT_EQ(lastRequestResourceId, testResourceId) << "request should be called with correct resource id";

	ASSERT_EQ(resource->resources, resources) << "returned resource should have correct resources";
	ASSERT_STREQ(resource->id, testResourceId) << "returned resource should have correct resource id";
	ASSERT_STREQ(resource->typeId, testTypeId) << "returned resource should have correct type id";
	ASSERT_EQ(resource->data, testDefaultResourceData) << "returned resource should have correct default data";
}

TEST_F(ShovelerResourcesTest, loadUnrequested)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	size_t testResourceBytes = 1337;

	shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBytes);
	ASSERT_EQ(lastLoadBuffer, &testResourceBuffer) << "load should be called with correct buffer";
	ASSERT_EQ(lastLoadBytes, testResourceBytes) << "load should be called with correct bytes";
}

TEST_F(ShovelerResourcesTest, freeLoaded)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	size_t testResourceBytes = 1337;
	const char *testResourceData = "test resource data";

	nextLoadResourceData = (void *) testResourceData;
	shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBytes);
	shovelerResourcesFree(resources);

	ASSERT_EQ(lastFreeResourceData, testResourceData) << "free resource data should be called with correct resource data";

	resources = NULL;
}

TEST_F(ShovelerResourcesTest, requestAndLoad)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	size_t testResourceBytes = 1337;
	const char *testResourceData = "test resource data";

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);

	nextLoadResourceData = (void *) testResourceData;
	shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBytes);

	ASSERT_EQ(resource->data, testResourceData) << "resource data should have changed after loading";
}

TEST_F(ShovelerResourcesTest, loadAndRequest)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	size_t testResourceBytes = 1337;
	const char *testResourceData = "test resource data";

	nextLoadResourceData = (void *) testResourceData;
	shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBytes);

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);

	ASSERT_EQ(resource->data, testResourceData) << "resource data should be set to correct loaded data";
}

static void requestResources(ShovelerResources *resources, const char *typeId, const char *resourceId, void *testPointer)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) testPointer;
	test->lastRequestResources = resources;
	test->lastRequestTypeId = typeId;
	test->lastRequestResourceId = resourceId;
}

void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, size_t bytes)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) typeLoader->data;
	test->lastLoadBuffer = buffer;
	test->lastLoadBytes = bytes;

	return test->nextLoadResourceData;
}

void freeResource(ShovelerResourcesTypeLoader *typeLoader, void *resourceData)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) typeLoader->data;
	test->lastFreeResourceData = resourceData;
}

void freeTypeLoader(ShovelerResourcesTypeLoader *typeLoader)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) typeLoader->data;
	test->freeTypeLoaderCalled = true;
}
