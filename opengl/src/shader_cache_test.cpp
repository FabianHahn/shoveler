#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/camera.h"
#include "shoveler/light.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/shader.h"
#include "shoveler/shader_cache.h"
}

class ShovelerShaderCacheTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		shaderKey = {&scene, &camera, &light, &model, &material, (void *) &userData};
		shaderKey2 = {&scene2, &camera2, &light2, &model2, &material2, (void *) &userData2};
		shader.key = shaderKey;
		shader2.key = shaderKey2;

		cache = shovelerShaderCacheCreateWithCustomFree(NULL);
	}

	virtual void TearDown()
	{
		shovelerShaderCacheFree(cache);
	}

	ShovelerScene scene;
	ShovelerScene scene2;
	ShovelerCamera camera;
	ShovelerCamera camera2;
	ShovelerLight light;
	ShovelerLight light2;
	ShovelerModel model;
	ShovelerModel model2;
	ShovelerMaterial material;
	ShovelerMaterial material2;
	int userData;
	int userData2;
	ShovelerShaderKey shaderKey;
	ShovelerShaderKey shaderKey2;
	ShovelerShader shader;
	ShovelerShader shader2;

	ShovelerShaderCache *cache;
};

TEST_F(ShovelerShaderCacheTest, insertAndLookup)
{
	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);

	shovelerShaderCacheInsert(cache, &shader);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_EQ(lookup2, &shader);
}

TEST_F(ShovelerShaderCacheTest, lookupAfterRemove)
{
	shovelerShaderCacheInsert(cache, &shader);
	bool removed = shovelerShaderCacheRemove(cache, &shaderKey);
	ASSERT_TRUE(removed);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
}

TEST_F(ShovelerShaderCacheTest, invalidateScene)
{
	ShovelerShaderKey shaderKey3 = {&scene, NULL, NULL, NULL, NULL, NULL};
	ShovelerShader shader3;
	shader3.key = shaderKey3;

	shovelerShaderCacheInsert(cache, &shader);
	shovelerShaderCacheInsert(cache, &shader2);
	shovelerShaderCacheInsert(cache, &shader3);
	shovelerShaderCacheInvalidateScene(cache, &scene);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey2);
	ASSERT_TRUE(lookup2 != NULL);
	ShovelerShader *lookup3 = shovelerShaderCacheLookup(cache, &shaderKey3);
	ASSERT_TRUE(lookup3 == NULL);
}

TEST_F(ShovelerShaderCacheTest, invalidateCamera)
{
	ShovelerShaderKey shaderKey3 = {NULL, &camera, NULL, NULL, NULL, NULL};
	ShovelerShader shader3;
	shader3.key = shaderKey3;

	shovelerShaderCacheInsert(cache, &shader);
	shovelerShaderCacheInsert(cache, &shader2);
	shovelerShaderCacheInsert(cache, &shader3);
	shovelerShaderCacheInvalidateCamera(cache, &camera);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey2);
	ASSERT_TRUE(lookup2 != NULL);
	ShovelerShader *lookup3 = shovelerShaderCacheLookup(cache, &shaderKey3);
	ASSERT_TRUE(lookup3 == NULL);
}

TEST_F(ShovelerShaderCacheTest, invalidateLight)
{
	ShovelerShaderKey shaderKey3 = {NULL, NULL, &light, NULL, NULL, NULL};
	ShovelerShader shader3;
	shader3.key = shaderKey3;

	shovelerShaderCacheInsert(cache, &shader);
	shovelerShaderCacheInsert(cache, &shader2);
	shovelerShaderCacheInsert(cache, &shader3);
	shovelerShaderCacheInvalidateLight(cache, &light);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey2);
	ASSERT_TRUE(lookup2 != NULL);
	ShovelerShader *lookup3 = shovelerShaderCacheLookup(cache, &shaderKey3);
	ASSERT_TRUE(lookup3 == NULL);
}

TEST_F(ShovelerShaderCacheTest, invalidateModel)
{
	ShovelerShaderKey shaderKey3 = {NULL, NULL, NULL, &model, NULL, NULL};
	ShovelerShader shader3;
	shader3.key = shaderKey3;

	shovelerShaderCacheInsert(cache, &shader);
	shovelerShaderCacheInsert(cache, &shader2);
	shovelerShaderCacheInsert(cache, &shader3);
	shovelerShaderCacheInvalidateModel(cache, &model);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey2);
	ASSERT_TRUE(lookup2 != NULL);
	ShovelerShader *lookup3 = shovelerShaderCacheLookup(cache, &shaderKey3);
	ASSERT_TRUE(lookup3 == NULL);
}

TEST_F(ShovelerShaderCacheTest, invalidateMaterial)
{
	ShovelerShaderKey shaderKey3 = {NULL, NULL, NULL, NULL, &material, NULL};
	ShovelerShader shader3;
	shader3.key = shaderKey3;

	shovelerShaderCacheInsert(cache, &shader);
	shovelerShaderCacheInsert(cache, &shader2);
	shovelerShaderCacheInsert(cache, &shader3);
	shovelerShaderCacheInvalidateMaterial(cache, &material);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey2);
	ASSERT_TRUE(lookup2 != NULL);
	ShovelerShader *lookup3 = shovelerShaderCacheLookup(cache, &shaderKey3);
	ASSERT_TRUE(lookup3 == NULL);
}

TEST_F(ShovelerShaderCacheTest, invalidateUserData)
{
	ShovelerShaderKey shaderKey3 = {NULL, NULL, NULL, NULL, NULL, (void *) &userData};
	ShovelerShader shader3;
	shader3.key = shaderKey3;

	shovelerShaderCacheInsert(cache, &shader);
	shovelerShaderCacheInsert(cache, &shader2);
	shovelerShaderCacheInsert(cache, &shader3);
	shovelerShaderCacheInvalidateUserData(cache, &userData);

	ShovelerShader *lookup = shovelerShaderCacheLookup(cache, &shaderKey);
	ASSERT_TRUE(lookup == NULL);
	ShovelerShader *lookup2 = shovelerShaderCacheLookup(cache, &shaderKey2);
	ASSERT_TRUE(lookup2 != NULL);
	ShovelerShader *lookup3 = shovelerShaderCacheLookup(cache, &shaderKey3);
	ASSERT_TRUE(lookup3 == NULL);
}
