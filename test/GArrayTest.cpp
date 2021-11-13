#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <glib.h>

#include <vector>

using namespace ::testing;

static std::vector<guint> clearedElements;

static void ClearFunc(void *data) {
	clearedElements.push_back(*static_cast<guint*>(data));
}

class GArrayTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		array = NULL;
	}

	virtual void TearDown()
	{
		if (array != NULL) {
			g_array_free(array, /* free_segment */ true);
		}
	}

	GArray *array;
};

TEST_F(GArrayTest, Fill)
{
	static constexpr guint numElements = 50;

	array = g_array_new(/* zero_terminated */ false, /* clear */ false, sizeof(guint));
	ASSERT_EQ(g_array_get_element_size(array), sizeof(int));
	ASSERT_EQ(array->len, 0);

	for (guint i = 0; i < numElements; i++) {
		g_array_append_val(array, i);
		ASSERT_EQ(array->len, i + 1);

		for (guint j = 0; j <= i; j++) {
			ASSERT_EQ(g_array_index(array, guint, j), j);
		}
	}
}

TEST_F(GArrayTest, FillCleared)
{
	static constexpr guint numElements = 50;

	array = g_array_new(/* zero_terminated */ false, /* clear */ true, sizeof(guint));
	ASSERT_EQ(g_array_get_element_size(array), sizeof(int));
	ASSERT_EQ(array->len, 0);

	for (guint i = 0; i < numElements; i++) {
		g_array_append_val(array, i);
		ASSERT_EQ(array->len, i + 1);

		for (guint j = 0; j <= i; j++) {
			ASSERT_EQ(g_array_index(array, guint, j), j);
		}
	}
}

TEST_F(GArrayTest, FillZeroTerminated)
{
	static constexpr guint numElements = 50;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	ASSERT_EQ(g_array_get_element_size(array), sizeof(int));
	ASSERT_EQ(array->len, 0);

	for (guint i = 0; i < numElements; i++) {
		g_array_append_val(array, i);
		ASSERT_EQ(array->len, i + 1);

		for (guint j = 0; j <= i; j++) {
			ASSERT_EQ(g_array_index(array, guint, j), j);
		}
		ASSERT_EQ(g_array_index(array, guint, i + 1), 0);
	}
}

TEST_F(GArrayTest, Sized)
{
	static constexpr guint numElements = 50;

	array = g_array_sized_new(/* zero_terminated */ false, /* clear */ true, sizeof(guint), numElements);
	ASSERT_EQ(g_array_get_element_size(array), sizeof(int));
	ASSERT_EQ(array->len, 0);

	gchar *originalData = array->data;

	for (guint i = 0; i < numElements; i++) {
		g_array_append_val(array, i);
		ASSERT_EQ(array->len, i + 1);
		ASSERT_EQ(array->data, originalData) << "assume no reallocation";
	}
}

TEST_F(GArrayTest, Copy)
{
	static constexpr guint numElements = 50;
	static constexpr guint testValue1 = 27;
	static constexpr guint testValue2 = 42;
	static constexpr guint testValue3 = 9001;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	for (guint i = 0; i < numElements; i++) {
		g_array_append_val(array, i);
	}

	GArray *copy = g_array_copy(array);
	ASSERT_EQ(copy->len, numElements);
	for (guint i = 0; i < numElements; i++) {
		ASSERT_EQ(g_array_index(array, guint, i), g_array_index(copy, guint, i));
	}

	g_array_index(array, guint, 17) = testValue1;
	ASSERT_NE(g_array_index(copy, guint, 17), testValue1);

	g_array_free(copy, /* free_segment */ true);

	ASSERT_EQ(array->len, numElements);
}

TEST_F(GArrayTest, Steal)
{
	static constexpr guint numElements = 50;
	static constexpr guint testValue1 = 27;
	static constexpr guint testValue2 = 42;
	static constexpr guint testValue3 = 9001;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	for (guint i = 0; i < numElements; i++) {
		g_array_append_val(array, i);
	}

	gchar *originalData = array->data;
	guint originalLength = array->len;

	gsize stolenLength = 0;
	gpointer stolenData = g_array_steal(array, &stolenLength);
	ASSERT_EQ(stolenData, originalData);
	ASSERT_EQ(stolenLength, originalLength);
	ASSERT_NE(array->data, stolenData);
	ASSERT_EQ(array->len, 0);

	g_array_append_val(array, testValue1);
	ASSERT_NE(*reinterpret_cast<guint *>(stolenData), testValue1);

	free(stolenData);
}

TEST_F(GArrayTest, RefUnref)
{
	static constexpr guint value = 27;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value);
	g_array_ref(array);
	g_array_unref(array);
	ASSERT_EQ(g_array_index(array, guint, 0), value);
	g_array_unref(array);
	array = NULL; // should be freed, will complain in valgrind otherwise
}

TEST_F(GArrayTest, GetElementSize)
{
	typedef struct {
		char *a;
		int b;
		double c;
		bool d;
	} CustomTestStruct;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(CustomTestStruct));
	ASSERT_EQ(g_array_get_element_size(array), sizeof(CustomTestStruct));
}

TEST_F(GArrayTest, AppendVals)
{
	static constexpr guint value = 99;
	static constexpr guint values[] = {1, 3, 6, 27, 42};

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value);
	g_array_append_vals(array, values, sizeof(values) / sizeof(values[0]));
	ASSERT_EQ(array->len, 6);

	for(int i = 0; i < array->len; i++) {
		if (i == 0) {
			ASSERT_EQ(g_array_index(array, guint, i), value);
		} else {
			ASSERT_EQ(g_array_index(array, guint, i), values[i-1]);
		}
	}
}

TEST_F(GArrayTest, PrependVals)
{
	static constexpr guint value = 99;
	static constexpr guint values[] = {1, 3, 6, 27, 42};

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value);
	g_array_prepend_vals(array, values, sizeof(values) / sizeof(values[0]));
	ASSERT_EQ(array->len, 6);

	for(int i = 0; i < array->len; i++) {
		if (i == 5) {
			ASSERT_EQ(g_array_index(array, guint, i), value);
		} else {
			ASSERT_EQ(g_array_index(array, guint, i), values[i]);
		}
	}
}

TEST_F(GArrayTest, InsertVals)
{
	static constexpr guint value = 99;
	static constexpr guint values[] = {1, 3, 6, 27, 42};

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value);
	g_array_append_val(array, value);
	g_array_insert_vals(array, 1, values, sizeof(values) / sizeof(values[0]));
	ASSERT_EQ(array->len, 7);

	for(int i = 0; i < array->len; i++) {
		if (i == 0 || i == 6) {
			ASSERT_EQ(g_array_index(array, guint, i), value);
		} else {
			ASSERT_EQ(g_array_index(array, guint, i), values[i-1]);
		}
	}
}

TEST_F(GArrayTest, RemoveIndex)
{
	static constexpr guint value1 = 1;
	static constexpr guint value2 = 2;
	static constexpr guint value3 = 3;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value1);
	g_array_append_val(array, value2);
	g_array_append_val(array, value3);
	g_array_remove_index(array, 0);
	ASSERT_EQ(array->len, 2);
	ASSERT_EQ(g_array_index(array, guint, 0), value2);
	ASSERT_EQ(g_array_index(array, guint, 1), value3);
}

TEST_F(GArrayTest, RemoveIndexFast)
{
	static constexpr guint value1 = 1;
	static constexpr guint value2 = 2;
	static constexpr guint value3 = 3;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value1);
	g_array_append_val(array, value2);
	g_array_append_val(array, value3);
	g_array_remove_index_fast(array, 0);
	ASSERT_EQ(array->len, 2);
	ASSERT_EQ(g_array_index(array, guint, 0), value3);
	ASSERT_EQ(g_array_index(array, guint, 1), value2);
}

TEST_F(GArrayTest, RemoveRange)
{
	static constexpr guint value1 = 1;
	static constexpr guint value2 = 2;
	static constexpr guint value3 = 3;
	static constexpr guint value4 = 4;

	array = g_array_new(/* zero_terminated */ true, /* clear */ false, sizeof(guint));
	g_array_append_val(array, value1);
	g_array_append_val(array, value2);
	g_array_append_val(array, value3);
	g_array_append_val(array, value4);
	g_array_remove_range(array, 1, 2);
	ASSERT_EQ(array->len, 2);
	ASSERT_EQ(g_array_index(array, guint, 0), value1);
	ASSERT_EQ(g_array_index(array, guint, 1), value4);
}

TEST_F(GArrayTest, SetSize)
{
	static constexpr guint value = 1;

	array = g_array_new(/* zero_terminated */ true, /* clear */ true, sizeof(guint));
	g_array_append_val(array, value);
	g_array_set_size(array, 10);
	ASSERT_EQ(array->len, 10);

	for(int i = 0; i < array->len; i++) {
		if (i == 0) {
			ASSERT_EQ(g_array_index(array, guint, i), value);
		} else {
			ASSERT_EQ(g_array_index(array, guint, i), 0);
		}
	}
}

TEST_F(GArrayTest, SetClearFunc)
{
	static constexpr guint value = 1;

	array = g_array_new(/* zero_terminated */ true, /* clear */ true, sizeof(guint));
	g_array_set_clear_func(array, ClearFunc);
	for(int i = 0; i < 10; i++) {
		g_array_append_val(array, i);
	}
	ASSERT_THAT(clearedElements, IsEmpty());

	g_array_remove_index(array, 1);
	ASSERT_THAT(clearedElements, SizeIs(1));
	g_array_remove_index_fast(array, 1);
	ASSERT_THAT(clearedElements, SizeIs(2));
	g_array_remove_range(array, 1, 2);
	ASSERT_THAT(clearedElements, SizeIs(4));
	g_array_free(array, /* freeSegment */ true);
	ASSERT_THAT(clearedElements, SizeIs(10));

	ASSERT_THAT(clearedElements, ElementsAre(1, 2, 9, 3, 0, 4, 5, 6, 7, 8));

	array = NULL;
}
