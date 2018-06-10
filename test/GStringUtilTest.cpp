#include <gtest/gtest.h>
#include <glib.h>

class GStringUtilTest : public ::testing::Test {
public:
	virtual void SetUp()
	{

	}

	virtual void TearDown()
	{

	}
};

TEST_F(GStringUtilTest, hasPrefix)
{
	const char *testData = "thebirdistheword";

	ASSERT_TRUE(g_str_has_prefix(testData, "the")) << "should pass happy case";
	ASSERT_TRUE(g_str_has_prefix(testData, "")) << "empty string should be a prefix";
	ASSERT_TRUE(g_str_has_prefix(testData, testData)) << "word should be a prefix of itself";
	ASSERT_FALSE(g_str_has_prefix(testData, "bird")) << "should fail unhappy case";
}

TEST_F(GStringUtilTest, hasSuffix)
{
	const char *testData = "thebirdistheword";

	ASSERT_TRUE(g_str_has_suffix(testData, "word")) << "should pass happy case";
	ASSERT_TRUE(g_str_has_suffix(testData, "")) << "empty string should be a suffix";
	ASSERT_TRUE(g_str_has_suffix(testData, testData)) << "word should be a suffix of itself";
	ASSERT_FALSE(g_str_has_suffix(testData, "bird")) << "should fail unhappy case";
}

TEST_F(GStringUtilTest, split)
{
	const char *testData = "the. bird. is. the. word";

	gchar **result = g_strsplit(testData, ". ", 0);
	ASSERT_STREQ(result[0], "the") << "first result element should match";
	ASSERT_STREQ(result[1], "bird") << "second result element should match";
	ASSERT_STREQ(result[2], "is") << "third result element should match";
	ASSERT_STREQ(result[3], "the") << "fourth result element should match";
	ASSERT_STREQ(result[4], "word") << "fifth result element should match";
	ASSERT_TRUE(result[5] == NULL) << "sixth result element should be NULL";

	g_strfreev(result);
}

TEST_F(GStringUtilTest, splitLimit)
{
	const char *testData = "the. bird. is. the. word";

	gchar **result = g_strsplit(testData, ". ", 3);
	ASSERT_STREQ(result[0], "the") << "first result element should match";
	ASSERT_STREQ(result[1], "bird") << "second result element should match";
	ASSERT_STREQ(result[2], "is. the. word") << "third result element should match";
	ASSERT_TRUE(result[3] == NULL) << "fourth result element should be NULL";

	g_strfreev(result);
}

TEST_F(GStringUtilTest, splitEmpty)
{
	gchar **result = g_strsplit("", "asdf", 1337);
	ASSERT_TRUE(result[0] == NULL) << "only result element should be NULL";

	g_strfreev(result);
}
