#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/resources.h"
}

static void requestResources(ShovelerResources *resources, const char *typeId, const char *resourceId, void *testPointer);
static void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, int bufferSize);
static void freeResourceData(ShovelerResourcesTypeLoader *typeLoader, void *resourceData);
static void freeTypeLoader(ShovelerResourcesTypeLoader *typeLoader);

class ShovelerResourcesTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		resources = shovelerResourcesCreate(requestResources, this);

		testTypeId = "test type id";
		testDefaultResourceData = "test default resource";

		testTypeLoader.typeId = testTypeId;
		testTypeLoader.defaultResourceData = (void *) testDefaultResourceData;
		testTypeLoader.data = this;
		testTypeLoader.load = loadResource;
		testTypeLoader.freeResourceData = freeResourceData;
		testTypeLoader.free = freeTypeLoader;
		bool typeLoaderRegistered = shovelerResourcesRegisterTypeLoader(resources, testTypeLoader);
		ASSERT_TRUE(typeLoaderRegistered) << "test type loader should register correctly";

		lastRequestResources = NULL;
		lastRequestTypeId = NULL;
		lastRequestResourceId = NULL;

		lastLoadBuffer = NULL;
		lastLoadBufferSize = 0;
		nextLoadResourceData = NULL;

		freeResourceDataArguments.clear();

		freeTypeLoaderCalled = false;
	}

	virtual void TearDown()
	{
		shovelerResourcesFree(resources);

		ASSERT_EQ(*freeResourceDataArguments.rbegin(), testTypeLoader.defaultResourceData) << "type loader's default resource data should be freed after resources was freed";
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
	int lastLoadBufferSize;
	void *nextLoadResourceData;

	std::vector<void *> freeResourceDataArguments;

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

TEST_F(ShovelerResourcesTest, requestInvalidType)
{
	const char *testInvalidTypeId = "foo/bar";
	const char *testResourceId = "test resource id";

	ShovelerResource *resource = shovelerResourcesGet(resources, testInvalidTypeId, testResourceId);
	ASSERT_TRUE(resource == NULL) << "request should have failed";
}

TEST_F(ShovelerResourcesTest, requestNull)
{
	const char *testResourceId = "test resource id";

	ShovelerResources *resources = shovelerResourcesCreate(NULL, NULL);
	bool typeLoaderRegistered = shovelerResourcesRegisterTypeLoader(resources, testTypeLoader);
	ASSERT_TRUE(typeLoaderRegistered) << "test type loader should register correctly";

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);
	ASSERT_TRUE(resource != NULL) << "returned resource must not be NULL";

	shovelerResourcesFree(resources);
}

TEST_F(ShovelerResourcesTest, loadUnrequested)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	int testResourceBufferSize = 1337;
	const char *testResourceData = "test resource data";

	nextLoadResourceData = (void *) testResourceData;
	bool loaded = shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBufferSize);
	ASSERT_TRUE(loaded) << "load should have succeeded";
	ASSERT_EQ(lastLoadBuffer, &testResourceBuffer) << "load should be called with correct buffer";
	ASSERT_EQ(lastLoadBufferSize, testResourceBufferSize) << "load should be called with correct bytes";
}

TEST_F(ShovelerResourcesTest, loadInvalidType)
{
	const char *testInvalidTypeId = "foo/bar";
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	int testResourceBufferSize = 1337;

	bool loaded = shovelerResourcesSet(resources, testInvalidTypeId, testResourceId, &testResourceBuffer, testResourceBufferSize);
	ASSERT_FALSE(loaded) << "load should have failed";
}

TEST_F(ShovelerResourcesTest, freeLoaded)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	int testResourceBufferSize = 1337;
	const char *testResourceData = "test resource data";

	nextLoadResourceData = (void *) testResourceData;
	shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBufferSize);
	shovelerResourcesFree(resources);

	ASSERT_EQ(*freeResourceDataArguments.begin(), testResourceData) << "free resource data should be called with correct resource data";

	resources = NULL;
}

TEST_F(ShovelerResourcesTest, requestAndLoad)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	int testResourceBufferSize = 1337;
	const char *testResourceData = "test resource data";

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);

	nextLoadResourceData = (void *) testResourceData;
	bool loaded = shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBufferSize);
	ASSERT_TRUE(loaded) << "load should have succeeded";

	ASSERT_EQ(resource->data, testResourceData) << "resource data should have changed after loading";
}


TEST_F(ShovelerResourcesTest, requestAndFailToLoad)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	int testResourceBufferSize = 1337;

	ShovelerResource *resource = shovelerResourcesGet(resources, testTypeId, testResourceId);

	nextLoadResourceData = NULL;
	bool loaded = shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBufferSize);
	ASSERT_FALSE(loaded) << "load should have failed";

	ASSERT_EQ(resource->data, testDefaultResourceData) << "resource data should be unchanged after failing to load";
}

TEST_F(ShovelerResourcesTest, loadAndRequest)
{
	const char *testResourceId = "test resource id";
	unsigned char testResourceBuffer = 42;
	int testResourceBufferSize = 1337;
	const char *testResourceData = "test resource data";

	nextLoadResourceData = (void *) testResourceData;
	shovelerResourcesSet(resources, testTypeId, testResourceId, &testResourceBuffer, testResourceBufferSize);

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

static void *loadResource(ShovelerResourcesTypeLoader *typeLoader, const unsigned char *buffer, int bufferSize)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) typeLoader->data;
	test->lastLoadBuffer = buffer;
	test->lastLoadBufferSize = bufferSize;

	return test->nextLoadResourceData;
}

static void freeResourceData(ShovelerResourcesTypeLoader *typeLoader, void *resourceData)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) typeLoader->data;
	test->freeResourceDataArguments.push_back(resourceData);
}

static void freeTypeLoader(ShovelerResourcesTypeLoader *typeLoader)
{
	ShovelerResourcesTest *test = (ShovelerResourcesTest *) typeLoader->data;
	test->freeTypeLoaderCalled = true;
}
