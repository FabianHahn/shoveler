#include <gtest/gtest.h>
#include <glib.h>

class GStringTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		string = g_string_new("");
	}

	virtual void TearDown()
	{
		if(string != NULL) {
			g_string_free(string, true);
		}
	}

	GString *string;
};

static char *generateLongString(gsize size)
{
	char *longString = (char *) malloc((size + 1) * sizeof(char));
	for(gsize i = 0; i < size; i++) {
		longString[i] = (char) 48 + (i % 75);
	}
	longString[size] = '\0';
	return longString;
}

static void call_g_string_vprintf(GString *string, const gchar *format, ...)
{
	va_list args;
	va_start(args, format);
	g_string_vprintf(string, format, args);
	va_end(args);
}

static void call_g_string_append_vprintf(GString *string, const gchar *format, ...)
{
	va_list args;
	va_start(args, format);
	g_string_append_vprintf(string, format, args);
	va_end(args);
}

TEST_F(GStringTest, newFree)
{
	const char *testData = "asdf";

	ASSERT_STREQ("", string->str) << "initial string should be equal to empty string";
	ASSERT_EQ(0, string->len) << "initial string length should be zero";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	gchar *segment = g_string_free(string, false);
	ASSERT_STREQ("", segment) << "preserved segment from freed initial string should be equal to empty string";
	g_free(segment);

	string = g_string_new(testData);
	ASSERT_NE(testData, string->str) << "initializing a string should copy input";
	ASSERT_STREQ(testData, string->str) << "initialized string should be equal to what it was initialized with";
	ASSERT_EQ(strlen(testData), string->len) << "initialized string should have correct length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";
}

TEST_F(GStringTest, newLen)
{
	gchar testData[4] = {'a', 's', 'd', 'f'};

	g_string_free(string, true);
	string = g_string_new_len(testData, 4);
	ASSERT_NE(testData, string->str) << "initializing a string should copy input";
	ASSERT_STREQ("asdf", string->str) << "initialized string should be equal to what it was initialized with";
	ASSERT_EQ(4, string->len) << "initialized string should have correct length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";
}

TEST_F(GStringTest, sizedNew)
{
	gsize testSize = 27;

	g_string_free(string, true);
	string = g_string_sized_new(testSize);
	ASSERT_STREQ("", string->str) << "initialized string should be empty";
	ASSERT_EQ(0, string->len) << "initialized string should have length zero";
	ASSERT_GT(string->allocated_len, testSize) << "allocated length must be larger than specified size";
}

TEST_F(GStringTest, assign)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_new(testData);

	gsize oldAllocatedLen = string->allocated_len;
	GString *sameString = g_string_assign(string, testData);
	ASSERT_EQ(string, sameString) << "assign should return original string pointer";
	ASSERT_STREQ(testData, string->str) << "assigned string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "assigned string should have correct length";
	ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";

	char *longString = generateLongString(2 * oldAllocatedLen);
	sameString = g_string_assign(string, longString);
	ASSERT_EQ(string, sameString) << "assign should return original string pointer";
	ASSERT_STREQ(longString, string->str) << "assigned string should match input";
	ASSERT_EQ(2 * oldAllocatedLen, string->len) << "assigned string should have correct length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	oldAllocatedLen = string->allocated_len;
	sameString = g_string_assign(string, testData);
	ASSERT_EQ(string, sameString) << "assign should return original string pointer";
	ASSERT_STREQ(testData, string->str) << "assigned string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "assigned string should have correct length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	free(longString);
}

TEST_F(GStringTest, vprintf)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_new(testData);

	gsize oldAllocatedLen = string->allocated_len;
	char *longString = generateLongString(2 * oldAllocatedLen);
	call_g_string_vprintf(string, "%s", longString);
	ASSERT_STREQ(longString, string->str) << "printed string should match input";
	ASSERT_EQ(2 * oldAllocatedLen, string->len) << "new length should be double the old allocated size";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	oldAllocatedLen = string->allocated_len;
	call_g_string_vprintf(string, "%s", testData);
	ASSERT_STREQ(testData, string->str) << "printed string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";
	ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";

	free(longString);
}

TEST_F(GStringTest, appendVprintf)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_sized_new((gsize) strlen(testData));
	gsize oldAllocatedLen = string->allocated_len;
	call_g_string_append_vprintf(string, "%s", testData);
	ASSERT_STREQ(testData, string->str) << "printed string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";
	ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";

	char *longString = generateLongString(oldAllocatedLen);
	GString *solutionString = g_string_new("");
	call_g_string_vprintf(solutionString, "%s%s", testData, longString);
	call_g_string_append_vprintf(string, "%s", longString);
	ASSERT_STREQ(solutionString->str, string->str) << "appended string should match solution";
	ASSERT_EQ(strlen(testData) + oldAllocatedLen, string->len) << "new length match expected length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	free(longString);
	g_string_free(solutionString, true);
}

TEST_F(GStringTest, printf)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_new(testData);

	gsize oldAllocatedLen = string->allocated_len;
	char *longString = generateLongString(2 * oldAllocatedLen);
	g_string_printf(string, "%s", longString);
	ASSERT_STREQ(longString, string->str) << "printed string should match input";
	ASSERT_EQ(2 * oldAllocatedLen, string->len) << "new length should be double the old allocated size";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	oldAllocatedLen = string->allocated_len;
	g_string_printf(string, "%s", testData);
	ASSERT_STREQ(testData, string->str) << "printed string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	free(longString);
}

TEST_F(GStringTest, appendPrintf)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_sized_new((gsize) strlen(testData));
	gsize oldAllocatedLen = string->allocated_len;
	g_string_append_printf(string, "%s", testData);
	ASSERT_STREQ(testData, string->str) << "printed string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";
	ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";

	char *longString = generateLongString(oldAllocatedLen);
	GString *solutionString = g_string_new("");
	g_string_printf(solutionString, "%s%s", testData, longString);
	g_string_append_printf(string, "%s", longString);
	ASSERT_STREQ(solutionString->str, string->str) << "appended string should match solution";
	ASSERT_EQ(strlen(testData) + oldAllocatedLen, string->len) << "new length match expected length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	free(longString);
	g_string_free(solutionString, true);
}

TEST_F(GStringTest, append)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_sized_new((gsize) strlen(testData));
	gsize oldAllocatedLen = string->allocated_len;
	GString *newString = g_string_append(string, testData);
	ASSERT_EQ(string, newString) << "append should always return input string pointer";
	ASSERT_STREQ(testData, string->str) << "printed string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";
	ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";

	char *longString = generateLongString(oldAllocatedLen);
	GString *solutionString = g_string_new("");
	g_string_printf(solutionString, "%s%s", testData, longString);
	newString = g_string_append(string, longString);
	ASSERT_EQ(string, newString) << "append should always return input string pointer";
	ASSERT_STREQ(solutionString->str, string->str) << "appended string should match solution";
	ASSERT_EQ(strlen(testData) + oldAllocatedLen, string->len) << "new length match expected length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	free(longString);
	g_string_free(solutionString, true);
}

TEST_F(GStringTest, appendC)
{
	g_string_free(string, true);
	string = g_string_sized_new(1);
	gsize oldAllocatedLen = string->allocated_len;
	GString *solutionString = g_string_new("");

	while(string->len < oldAllocatedLen - 1) {
		g_string_append(solutionString, "x");
		GString *newString = g_string_append_c(string, 'x');
		ASSERT_EQ(string, newString) << "append should always return input string pointer";
		ASSERT_STREQ(solutionString->str, string->str) << "appended string should match solution";
		ASSERT_EQ(solutionString->len, string->len) << "appended string length should match solution length";
		ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";
	}

	g_string_append(solutionString, "y");
	GString *newString = g_string_append_c(string, 'y');
	ASSERT_EQ(string, newString) << "append should always return input string pointer";
	ASSERT_STREQ(solutionString->str, string->str) << "appended string should match solution";
	ASSERT_EQ(solutionString->len, string->len) << "appended string length should match solution length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	g_string_free(solutionString, true);
}

TEST_F(GStringTest, appendLen)
{
	const char *testData = "asdf";

	g_string_free(string, true);
	string = g_string_sized_new((gsize) strlen(testData));
	gsize oldAllocatedLen = string->allocated_len;
	GString *newString = g_string_append_len(string, testData, (gsize) strlen(testData));
	ASSERT_EQ(string, newString) << "append should always return input string pointer";
	ASSERT_STREQ(testData, string->str) << "printed string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";
	ASSERT_EQ(oldAllocatedLen, string->allocated_len) << "allocated length should be unchanged";

	char *longString = generateLongString(oldAllocatedLen);
	GString *solutionString = g_string_new("");
	g_string_printf(solutionString, "%s%s", testData, longString);
	newString = g_string_append_len(string, longString, oldAllocatedLen);
	ASSERT_EQ(string, newString) << "append should always return input string pointer";
	ASSERT_STREQ(solutionString->str, string->str) << "appended string should match solution";
	ASSERT_EQ(strlen(testData) + oldAllocatedLen, string->len) << "new length match expected length";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";

	free(longString);
	g_string_free(solutionString, true);
}

TEST_F(GStringTest, truncate)
{
	const char *testData = "asdf";

	g_string_append(string, testData);
	GString *newString = g_string_truncate(string, 1337);
	ASSERT_EQ(string, newString) << "truncate should always return input string pointer";
	ASSERT_STREQ(testData, string->str) << "untruncated string should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";

	newString = g_string_truncate(string, 1);
	ASSERT_EQ(string, newString) << "truncate should always return input string pointer";
	ASSERT_EQ(testData[0], string->str[0]) << "truncated string should match solution";
	ASSERT_EQ(1, string->len) << "truncated length should be as requested";
}

TEST_F(GStringTest, setSize)
{
	const char *testData = "asdf";

	g_string_append(string, testData);
	GString *newString = g_string_set_size(string, (gsize) strlen(testData));
	ASSERT_EQ(string, newString) << "set size should always return input string pointer";
	ASSERT_STREQ(testData, string->str) << "string with same size should match input";
	ASSERT_EQ(strlen(testData), string->len) << "string length should match input length";

	newString = g_string_set_size(string, 1);
	ASSERT_EQ(string, newString) << "set size should always return input string pointer";
	ASSERT_EQ(testData[0], string->str[0]) << "truncated string should match solution";
	ASSERT_EQ(1, string->len) << "truncated length should be as requested";

	newString = g_string_set_size(string, 27);
	ASSERT_EQ(string, newString) << "set size should always return input string pointer";
	ASSERT_EQ('\0', string->str[27]) << "set size should result in null terminated string";
	ASSERT_EQ(27, string->len) << "set size should resize string to expected size";
	ASSERT_GT(string->allocated_len, string->len) << "allocated length must be larger than length";
}
