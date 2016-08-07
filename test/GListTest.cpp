#include <gtest/gtest.h>
#include <glib.h>

#include <vector>

static std::vector<gpointer> freeCallbacks;
static std::vector<gconstpointer> copyCallbacks;
static std::vector<gpointer> foreachCallbacks;
static gpointer copyLastUserData;
static gpointer foreachLastUserData;
static gpointer compareLastUserData;

class GListTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		list = NULL;
	}

	virtual void TearDown()
	{
		g_list_free(list);
	}

	GList *list;
};

static gint test_compare_int_with_data(gconstpointer v1, gconstpointer v2, gpointer userData)
{
	compareLastUserData = userData;

	if(v1 == NULL && v2 == NULL) {
		return 0;
	}
	if(v1 == NULL) {
		return -1;
	}
	if(v2 == NULL) {
		return 1;
	}

	const int& firstInt = *reinterpret_cast<const int *>(v1);
	const int& secondInt = *reinterpret_cast<const int *>(v2);

	if(firstInt > secondInt) {
		return 1;
	}
	if(firstInt == secondInt) {
		return 0;
	}
	return -1;
}

static gint test_compare_int(gconstpointer v1, gconstpointer v2)
{
	return test_compare_int_with_data(v1, v2, NULL);
}

static void test_free_callback(gpointer data)
{
	freeCallbacks.push_back(data);
}

static void test_copy_clear()
{
	copyCallbacks.clear();
	copyLastUserData = NULL;
}

static gpointer test_copy_callback(gconstpointer src, gpointer userData)
{
	copyCallbacks.push_back(src);
	copyLastUserData = userData;
	return userData;
}

static void test_foreach_callback(gpointer data, gpointer userData)
{
	foreachLastUserData = userData;
	foreachCallbacks.push_back(data);
}

TEST_F(GListTest, append)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after appending an element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next == NULL) << "first list element should not have a next element";
	list = g_list_append(list, &testData2);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after appending an element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next != NULL) << "first list element should have a next element";
	GList *secondElement = list->next;
	ASSERT_EQ(&testData2, secondElement->data) << "second list element data should be set";
	ASSERT_TRUE(secondElement->prev == list) << "second list element should have the first as previous element";
	ASSERT_TRUE(secondElement->next == NULL) << "second list element should not have a next element";
}

TEST_F(GListTest, prepend)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_prepend(list, &testData1);
	GList *first = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after prepending an element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next == NULL) << "first list element should not have a next element";
	list = g_list_prepend(list, &testData2);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after prepending an element";
	ASSERT_EQ(&testData2, list->data) << "second list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "second list element should not have a previous element";
	ASSERT_EQ(first, list->next) << "second list element should have first as next element";
}

TEST_F(GListTest, insert)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testData3 = 27;

	list = g_list_insert(list, &testData1, -1);
	GList *first = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_EQ(&testData1, first->data) << "first list element data should be set";
	ASSERT_TRUE(first->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(first->next == NULL) << "first list element should not have a next element";

	list = g_list_insert(list, &testData2, 0);
	GList *second = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_NE(first, second) << "second list element should not be equal to first list element";
	ASSERT_EQ(&testData2, second->data) << "second list element data should be set";
	ASSERT_TRUE(second->prev == NULL) << "second list element should not have a previous element";
	ASSERT_EQ(first, second->next) << "second list element should have first as next element";
	ASSERT_EQ(second, first->prev) << "first list element should have second as previous element";

	list = g_list_insert(list, &testData3, 1);
	GList *third = list->next;
	ASSERT_EQ(second, list) << "list head element should still be second after inserting third element";
	ASSERT_EQ(&testData3, third->data) << "third list element data should be set";
	ASSERT_EQ(first, third->next) << "third list element should have first as next element";
	ASSERT_EQ(second, third->prev) << "third list element should have second as previous element";
	ASSERT_EQ(third, second->next) << "second list element should have third as next element";
	ASSERT_EQ(third, first->prev) << "first list element should have third as previous element";
}

TEST_F(GListTest, insertBefore)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testData3 = 27;

	list = g_list_insert_before(list, NULL, &testData1);
	GList *first = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_EQ(&testData1, first->data) << "first list element data should be set";
	ASSERT_TRUE(first->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(first->next == NULL) << "first list element should not have a next element";

	list = g_list_insert_before(list, first, &testData2);
	GList *second = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_NE(first, second) << "second list element should not be equal to first list element";
	ASSERT_EQ(&testData2, second->data) << "second list element data should be set";
	ASSERT_TRUE(second->prev == NULL) << "second list element should not have a previous element";
	ASSERT_EQ(first, second->next) << "second list element should have first as next element";
	ASSERT_EQ(second, first->prev) << "first list element should have second as previous element";

	list = g_list_insert_before(list, first, &testData3);
	GList *third = list->next;
	ASSERT_EQ(second, list) << "list head element should still be second after inserting third element";
	ASSERT_EQ(&testData3, third->data) << "third list element data should be set";
	ASSERT_EQ(first, third->next) << "third list element should have first as next element";
	ASSERT_EQ(second, third->prev) << "third list element should have second as previous element";
	ASSERT_EQ(third, second->next) << "second list element should have third as next element";
	ASSERT_EQ(third, first->prev) << "first list element should have third as previous element";
}

TEST_F(GListTest, insertSorted)
{
	int testData1 = 2;
	int testData2 = 0;
	int testData3 = 1;
	int testData4 = 3;

	list = g_list_insert_sorted(list, &testData1, test_compare_int);
	GList *first = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_EQ(&testData1, first->data) << "first list element data should be set";
	ASSERT_TRUE(first->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(first->next == NULL) << "first list element should not have a next element";

	list = g_list_insert_sorted(list, &testData2, test_compare_int);
	GList *second = list;
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_NE(first, second) << "second list element should not be equal to first list element";
	ASSERT_EQ(&testData2, second->data) << "second list element data should be set";
	ASSERT_TRUE(second->prev == NULL) << "second list element should not have a previous element";
	ASSERT_EQ(first, second->next) << "second list element should have first as next element";
	ASSERT_EQ(second, first->prev) << "first list element should have second as previous element";

	list = g_list_insert_sorted(list, &testData3, test_compare_int);
	GList *third = list->next;
	ASSERT_EQ(second, list) << "list head element should still be second after inserting third element";
	ASSERT_EQ(&testData3, third->data) << "third list element data should be set";
	ASSERT_EQ(first, third->next) << "third list element should have first as next element";
	ASSERT_EQ(second, third->prev) << "third list element should have second as previous element";
	ASSERT_EQ(third, second->next) << "second list element should have third as next element";
	ASSERT_EQ(third, first->prev) << "first list element should have third as previous element";

	list = g_list_insert_sorted(list, &testData4, test_compare_int);
	GList *fourth = first->next;
	ASSERT_EQ(second, list) << "list head element should still be second after inserting fourth element";
	ASSERT_EQ(&testData4, fourth->data) << "fourth list element data should be set";
	ASSERT_TRUE(fourth->next == NULL) << "fourth list element should not have a next element";
	ASSERT_EQ(first, fourth->prev) << "fourth list element should have first as previous element";
	ASSERT_EQ(fourth, first->next) << "first list element should have fourth as next element";
}

TEST_F(GListTest, remove)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_remove(list, &testData2);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after removing second element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next == NULL) << "first list element should not have a next element";
	list = g_list_remove(list, &testData1);
	ASSERT_TRUE(list == NULL) << "list should be NULL after removing first element";
}

TEST_F(GListTest, removeLink)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	GList *secondElement = list->next;
	list = g_list_remove_link(list, secondElement);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after removing second element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next == NULL) << "first list element should not have a next element";
	ASSERT_EQ(&testData2, secondElement->data) << "second list element data should be set";
	ASSERT_TRUE(secondElement->prev == NULL) << "second list element should not have a previous element";
	ASSERT_TRUE(secondElement->next == NULL) << "second list element should not have a next element";
	GList *firstElement = list;
	list = g_list_remove_link(list, firstElement);
	ASSERT_TRUE(list == NULL) << "list should be NULL after removing first element";
	ASSERT_EQ(&testData1, firstElement->data) << "first list element data should be set";
	ASSERT_TRUE(firstElement->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(firstElement->next == NULL) << "first list element should not have a next element";
}

TEST_F(GListTest, deleteLink)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	GList *secondElement = list->next;
	list = g_list_delete_link(list, secondElement);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after removing second element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next == NULL) << "first list element should not have a next element";
	GList *firstElement = list;
	list = g_list_remove_link(list, firstElement);
	ASSERT_TRUE(list == NULL) << "list should be NULL after removing first element";
}

TEST_F(GListTest, removeAll)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	list = g_list_remove_all(list, &testData2);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after removing second element";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next != NULL) << "first list element should have a next element";
	ASSERT_EQ(&testData1, list->next->data) << "second list element data should be set";
	ASSERT_TRUE(list->next->prev == list) << "second list element should have first as previous element";
	ASSERT_TRUE(list->next->next == NULL) << "second list element should not have a next element";

	list = g_list_remove_all(list, &testData1);
	ASSERT_TRUE(list == NULL) << "list should be NULL after removing all elements";
}

TEST_F(GListTest, freeFull)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	freeCallbacks.clear();
	g_list_free_full(list, test_free_callback);
	list = NULL;
	std::vector<gpointer> expectedCallbacks = {&testData1, &testData2, &testData1, &testData2};
	ASSERT_EQ(expectedCallbacks, freeCallbacks) << "actual callback list should match expected";
}

TEST_F(GListTest, alloc)
{
	GList *alloc = g_list_alloc();
	ASSERT_TRUE(alloc->data == NULL) << "allocated element data should not be set";
	ASSERT_TRUE(alloc->prev == NULL) << "allocated element should not have a previous element";
	ASSERT_TRUE(alloc->next == NULL) << "allocated element should not have a next element";
	g_list_free(alloc);
}

TEST_F(GListTest, length)
{
	guint length = g_list_length(list);
	ASSERT_EQ(0, length) << "list length should be zero before inserting elements";
	list = g_list_append(list, NULL);
	length = g_list_length(list);
	ASSERT_EQ(1, length) << "list length should be one after inserting an element";
	list = g_list_append(list, NULL);
	length = g_list_length(list);
	ASSERT_EQ(2, length) << "list length should be two after inserting another element";
	list = g_list_remove(list, NULL);
	length = g_list_length(list);
	ASSERT_EQ(1, length) << "list length should be one after deleting an element";
}

TEST_F(GListTest, copy)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	GList *copiedList = g_list_copy(list);
	ASSERT_TRUE(copiedList != NULL) << "copied list should not be NULL";
	ASSERT_EQ(&testData1, copiedList->data) << "copied first list element data should be set";
	ASSERT_TRUE(copiedList->prev == NULL) << "copied first list element should not have a previous element";
	ASSERT_TRUE(copiedList->next != NULL) << "copied first list element should have a next element";
	GList *secondElement = copiedList->next;
	ASSERT_EQ(&testData2, secondElement->data) << "copied second list element data should be set";
	ASSERT_EQ(copiedList, secondElement->prev) << "copied second list element should have the first as previous element";
	ASSERT_TRUE(secondElement->next == NULL) << "copied second list element should not have a next element";
	ASSERT_NE(list, copiedList) << "copied list should not be equal to original list";
	ASSERT_NE(list->next, copiedList->next) << "copied list second element should not be equal to original list second element";
	g_list_free(copiedList);
}

TEST_F(GListTest, copyDeep)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testUserData = 27;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	test_copy_clear();
	GList *copiedList = g_list_copy_deep(list, test_copy_callback, &testUserData);
	ASSERT_TRUE(copiedList != NULL) << "copied list should not be NULL";
	ASSERT_EQ(&testUserData, copiedList->data) << "copied first list element data should be set";
	ASSERT_TRUE(copiedList->prev == NULL) << "copied first list element should not have a previous element";
	ASSERT_TRUE(copiedList->next != NULL) << "copied first list element should have a next element";
	GList *secondElement = copiedList->next;
	ASSERT_EQ(&testUserData, secondElement->data) << "copied second list element data should be set";
	ASSERT_EQ(copiedList, secondElement->prev) << "copied second list element should have the first as previous element";
	ASSERT_TRUE(secondElement->next == NULL) << "copied second list element should not have a next element";
	ASSERT_NE(list, copiedList) << "copied list should not be equal to original list";
	ASSERT_NE(list->next, copiedList->next) << "copied list second element should not be equal to original list second element";
	g_list_free(copiedList);

	std::vector<gconstpointer> expectedCallbacks = {&testData1, &testData2};
	ASSERT_EQ(expectedCallbacks, copyCallbacks) << "actual callback list should match expected";
	ASSERT_EQ(copyLastUserData, &testUserData) << "last copy user data should match provided user data";
}

TEST_F(GListTest, reverse)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testData3 = 27;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData3);
	
	GList *reversedList = g_list_reverse(list);
	ASSERT_TRUE(reversedList != NULL) << "reversed list should not be NULL after inserting an element";
	ASSERT_EQ(&testData3, reversedList->data) << "first reversed list element data should be set";
	ASSERT_TRUE(reversedList->prev == NULL) << "first reversed list element should not have a previous element";
	ASSERT_TRUE(reversedList->next != NULL) << "first reversed list element should have a next element";
	ASSERT_EQ(&testData2, reversedList->next->data) << "second reversed list element data should be set";
	ASSERT_EQ(reversedList, reversedList->next->prev) << "second reversed list element should have first element as previous";
	ASSERT_TRUE(reversedList->next->next != NULL) << "second list element should have a next element";
	ASSERT_EQ(&testData1, reversedList->next->next->data) << "third reversed list element data should be set";
	ASSERT_EQ(reversedList->next, reversedList->next->next->prev) << "third reversed list element should have second element as previous";
	ASSERT_TRUE(reversedList->next->next->next == NULL) << "third reversed list element should not have a next element";
	GList *reversedReversedList = g_list_reverse(reversedList);
	ASSERT_EQ(list, reversedReversedList) << "reverse of reversed list should result in original list";
}

TEST_F(GListTest, sort)
{
	int testData1 = 42;
	int testData2 = 1;
	int testData3 = 27;
	int testData4 = 27;
	int testData5 = 5;
	int testData6 = 3;
	int testData7 = 2;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData3);
	list = g_list_append(list, &testData4);
	list = g_list_append(list, &testData5);
	list = g_list_append(list, &testData6);
	list = g_list_append(list, &testData7);

	list = g_list_sort(list, test_compare_int);
	std::vector<gpointer> expectedSolution = {&testData2, &testData7, &testData6, &testData5, &testData3, &testData4, &testData1};
	GList *previous = NULL;
	int i = 0;
	for(GList *iter = list; iter != NULL; previous = iter, iter = iter->next, i++) {
		ASSERT_EQ(expectedSolution[i], iter->data) << "sorted list element " << i << " should have expected value";
		ASSERT_EQ(previous, iter->prev) << "sorted list element " << i << " previous pointer should be correct";
	}

	list = g_list_sort(list, test_compare_int);
	previous = NULL;
	i = 0;
	for(GList *iter = list; iter != NULL; previous = iter, iter = iter->next, i++) {
		ASSERT_EQ(expectedSolution[i], iter->data) << "resorted list element " << i << " should have expected value";
		ASSERT_EQ(previous, iter->prev) << "resorted list element " << i << " previous pointer should be correct";
	}
}

TEST_F(GListTest, insertSortedWithData)
{
	int testData1 = 2;
	int testData2 = 0;
	int testData3 = 1;
	int testData4 = 3;
	int testUserData = 1337;

	compareLastUserData = NULL;
	list = g_list_insert_sorted_with_data(list, &testData1, test_compare_int_with_data, &testUserData);
	GList *first = list;
	ASSERT_TRUE(compareLastUserData == NULL) << "user data should not have been passed when sorted inserting into empty list";
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_EQ(&testData1, first->data) << "first list element data should be set";
	ASSERT_TRUE(first->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(first->next == NULL) << "first list element should not have a next element";

	compareLastUserData = NULL;
	list = g_list_insert_sorted_with_data(list, &testData2, test_compare_int_with_data, &testUserData);
	GList *second = list;
	ASSERT_EQ(&testUserData, compareLastUserData) << "passed user data should have expected value";
	ASSERT_TRUE(list != NULL) << "list should not be NULL after inserting an element";
	ASSERT_NE(first, second) << "second list element should not be equal to first list element";
	ASSERT_EQ(&testData2, second->data) << "second list element data should be set";
	ASSERT_TRUE(second->prev == NULL) << "second list element should not have a previous element";
	ASSERT_EQ(first, second->next) << "second list element should have first as next element";
	ASSERT_EQ(second, first->prev) << "first list element should have second as previous element";

	compareLastUserData = NULL;
	list = g_list_insert_sorted_with_data(list, &testData3, test_compare_int_with_data, &testUserData);
	GList *third = list->next;
	ASSERT_EQ(&testUserData, compareLastUserData) << "passed user data should have expected value";
	ASSERT_EQ(second, list) << "list head element should still be second after inserting third element";
	ASSERT_EQ(&testData3, third->data) << "third list element data should be set";
	ASSERT_EQ(first, third->next) << "third list element should have first as next element";
	ASSERT_EQ(second, third->prev) << "third list element should have second as previous element";
	ASSERT_EQ(third, second->next) << "second list element should have third as next element";
	ASSERT_EQ(third, first->prev) << "first list element should have third as previous element";

	compareLastUserData = NULL;
	list = g_list_insert_sorted_with_data(list, &testData4, test_compare_int_with_data, &testUserData);
	GList *fourth = first->next;
	ASSERT_EQ(&testUserData, compareLastUserData) << "passed user data should have expected value";
	ASSERT_EQ(second, list) << "list head element should still be second after inserting fourth element";
	ASSERT_EQ(&testData4, fourth->data) << "fourth list element data should be set";
	ASSERT_TRUE(fourth->next == NULL) << "fourth list element should not have a next element";
	ASSERT_EQ(first, fourth->prev) << "fourth list element should have first as previous element";
	ASSERT_EQ(fourth, first->next) << "first list element should have fourth as next element";
}

TEST_F(GListTest, sortWithData)
{
	int testData1 = 42;
	int testData2 = 1;
	int testData3 = 27;
	int testData4 = 27;
	int testData5 = 5;
	int testData6 = 3;
	int testData7 = 2;
	int testUserData = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData3);
	list = g_list_append(list, &testData4);
	list = g_list_append(list, &testData5);
	list = g_list_append(list, &testData6);
	list = g_list_append(list, &testData7);

	compareLastUserData = NULL;
	list = g_list_sort_with_data(list, test_compare_int_with_data, &testUserData);
	ASSERT_EQ(&testUserData, compareLastUserData) << "passed user data should have expected value";
	std::vector<gpointer> expectedSolution = {&testData2, &testData7, &testData6, &testData5, &testData3, &testData4, &testData1};
	GList *previous = NULL;
	int i = 0;
	for(GList *iter = list; iter != NULL; previous = iter, iter = iter->next, i++) {
		ASSERT_EQ(expectedSolution[i], iter->data) << "sorted list element " << i << " should have expected value";
		ASSERT_EQ(previous, iter->prev) << "sorted list element " << i << " previous pointer should be correct";
	}

	compareLastUserData = NULL;
	list = g_list_sort_with_data(list, test_compare_int_with_data, &testUserData);
	ASSERT_EQ(&testUserData, compareLastUserData) << "passed user data should have expected value";
	previous = NULL;
	i = 0;
	for(GList *iter = list; iter != NULL; previous = iter, iter = iter->next, i++) {
		ASSERT_EQ(expectedSolution[i], iter->data) << "resorted list element " << i << " should have expected value";
		ASSERT_EQ(previous, iter->prev) << "resorted list element " << i << " previous pointer should be correct";
	}
}

TEST_F(GListTest, concat)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	GList *secondList = g_list_append(NULL, &testData2);
	list = g_list_concat(list, secondList);
	ASSERT_TRUE(list != NULL) << "list should not be NULL after concatenating";
	ASSERT_EQ(&testData1, list->data) << "first list element data should be set";
	ASSERT_TRUE(list->prev == NULL) << "first list element should not have a previous element";
	ASSERT_TRUE(list->next != NULL) << "first list element should have a next element";
	ASSERT_EQ(&testData2, list->next->data) << "second list element data should be set";
	ASSERT_TRUE(list->next->prev == list) << "second list element should have first as previous element";
	ASSERT_TRUE(list->next->next == NULL) << "second list element should not have a next element";
}

TEST_F(GListTest, foreach)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testUserData = 27;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData2);

	foreachLastUserData = NULL;
	g_list_foreach(list, test_foreach_callback, &testUserData);
	ASSERT_EQ(&testUserData, foreachLastUserData) << "passed user data should have expected value";
	std::vector<gpointer> expectedCallbacks = {&testData1, &testData2, &testData2};
	ASSERT_EQ(expectedCallbacks, foreachCallbacks) << "actual callback list should match expected";
}

TEST_F(GListTest, first)
{
	int testData = 42;

	GList *first = g_list_first(list);
	ASSERT_TRUE(first == NULL) << "first element of empty list should be NULL";
	list = g_list_append(list, &testData);
	first = g_list_first(list);
	ASSERT_TRUE(first != NULL) << "first element of appended list should not be NULL";
	ASSERT_EQ(list, first) << "first element of appended list should be equal to list";
	list = g_list_append(list, &testData);
	first = g_list_first(list);
	ASSERT_EQ(list, first) << "first element of list should be unchanged after appending an element";
}

TEST_F(GListTest, last)
{
	int testData = 42;

	GList *last = g_list_last(list);
	ASSERT_TRUE(last == NULL) << "last element of empty list should be NULL";
	list = g_list_append(list, &testData);
	last = g_list_last(list);
	ASSERT_TRUE(last != NULL) << "last element of appended list should not be NULL";
	ASSERT_EQ(list, last) << "last element of appended list should be equal to list";
	list = g_list_append(list, &testData);
	last = g_list_last(list);
	ASSERT_NE(list, last) << "last element of list should be changed after appending an element";
}

TEST_F(GListTest, nth)
{
	int testData = 42;

	GList *nth = g_list_nth(list, 0);
	ASSERT_TRUE(nth == NULL) << "nth should return NULL on empty list";

	list = g_list_append(list, &testData);
	list = g_list_append(list, &testData);
	list = g_list_append(list, &testData);

	nth = g_list_nth(list, 0);
	ASSERT_EQ(list, nth) << "0th element should be found correctly";
	nth = g_list_nth(list, 1);
	ASSERT_EQ(list->next, nth) << "1st element should be found correctly";
	nth = g_list_nth(list, 2);
	ASSERT_EQ(list->next->next, nth) << "2nd element should be found correctly";
	nth = g_list_nth(list, 3);
	ASSERT_TRUE(nth == NULL) << "3rd element should be out of bounds";
}

TEST_F(GListTest, nthData)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testData3 = 27;

	gpointer nthData = g_list_nth_data(list, 0);
	ASSERT_TRUE(nthData == NULL) << "nth data should return NULL on empty list";

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	list = g_list_append(list, &testData3);

	nthData = g_list_nth_data(list, 0);
	ASSERT_EQ(&testData1, nthData) << "0th element data should be found correctly";
	nthData = g_list_nth_data(list, 1);
	ASSERT_EQ(&testData2, nthData) << "1st element data should be found correctly";
	nthData = g_list_nth_data(list, 2);
	ASSERT_EQ(&testData3, nthData) << "2nd element data should be found correctly";
	nthData = g_list_nth_data(list, 3);
	ASSERT_TRUE(nthData == NULL) << "3rd element data should be out of bounds";
}

TEST_F(GListTest, nthPrev)
{
	int testData1 = 42;
	int testData2 = 1337;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);
	
	GList *nthPrev = g_list_nth_prev(list->next, 0);
	ASSERT_EQ(list->next, nthPrev) << "zero nth previous element should return same element";
	nthPrev = g_list_nth_prev(list->next, 1);
	ASSERT_EQ(list, nthPrev) << "one nth previous element should return previous element";
	nthPrev = g_list_nth_prev(list->next, 2);
	ASSERT_TRUE(nthPrev == NULL) << "two nth previous element should be out of bounds";
}

TEST_F(GListTest, find)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testData3 = 27;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	GList *find = g_list_find(list, &testData1);
	ASSERT_EQ(list, find) << "first element should be found";
	find = g_list_find(list->next, &testData2);
	ASSERT_EQ(list->next, find) << "second element should be found";
	find = g_list_find(list->next, &testData3);
	ASSERT_TRUE(find == NULL) << "unknown element should not be found";
}

TEST_F(GListTest, findCustom)
{
	int testData1 = 42;
	int testData2 = 1337;
	int searchData1 = testData1;
	int searchData2 = testData2;
	int searchData3 = 27;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	GList *find = g_list_find_custom(list, &searchData1, test_compare_int);
	ASSERT_EQ(list, find) << "first element should be found";
	find = g_list_find_custom(list->next, &searchData2, test_compare_int);
	ASSERT_EQ(list->next, find) << "second element should be found";
	find = g_list_find_custom(list->next, &searchData3, test_compare_int);
	ASSERT_TRUE(find == NULL) << "unknown element should not be found";
}

TEST_F(GListTest, index)
{
	int testData1 = 42;
	int testData2 = 1337;
	int testData3 = 27;

	list = g_list_append(list, &testData1);
	list = g_list_append(list, &testData2);

	gint index = g_list_index(list, &testData1);
	ASSERT_EQ(0, index) << "index of first element should be zero";
	index = g_list_index(list, &testData2);
	ASSERT_EQ(1, index) << "index of second element should be one";
	index = g_list_index(list, &testData3);
	ASSERT_EQ(-1, index) << "index of unknown element should be minus one";
}
