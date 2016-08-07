#include <gtest/gtest.h>
#include <glib.h>

#include <algorithm>
#include <vector>

static guint test_hash(gconstpointer key);
static gboolean test_equal(gconstpointer a, gconstpointer b);
static void test_free_key(gpointer data);
static void test_free_value(gpointer data);

class GHashTableTest;
static GHashTableTest *currentTest = NULL;
static std::vector < std::pair<gpointer, gpointer> > foreachCallbacks;
static gpointer foreachLastUserData;

class GHashTableTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		hash = g_str_hash;
		equal = g_str_equal;
		keyDestroy = NULL;
		valueDestroy = NULL;
		hashTable = NULL;
		created = false;

		currentTest = this;
	}

	virtual void Create()
	{
		hashTable = g_hash_table_new_full(test_hash, test_equal, test_free_key, test_free_value);
		created = true;
	}

	virtual void Destroy()
	{
		if(created) {
			g_hash_table_destroy(hashTable);
			hashTable = NULL;
			created = false;
		}
	}

	virtual void TearDown()
	{
		Destroy();

		hashed.clear();
		equaled.clear();
		keyDestroyed.clear();
		valueDestroyed.clear();

		currentTest = NULL;
	}

	GHashFunc hash;
	GEqualFunc equal;
	GDestroyNotify keyDestroy;
	GDestroyNotify valueDestroy;
	GHashTable *hashTable;
	bool created;
	std::vector<gconstpointer> hashed;
	std::vector< std::pair<gconstpointer, gconstpointer> > equaled;
	std::vector<gpointer> keyDestroyed;
	std::vector<gpointer> valueDestroyed;
};

static guint test_hash(gconstpointer key)
{
	currentTest->hashed.push_back(key);
	return currentTest->hash(key);
}

template <typename T>
static std::pair<T, T> make_ordered_pair(T a, T b) {
	if(std::less<T>{}(a, b)) {
		return std::make_pair(a, b);
	} else {
		return std::make_pair(b, a);
	}
}

static gboolean test_equal(gconstpointer a, gconstpointer b)
{
	currentTest->equaled.push_back(make_ordered_pair(a, b));
	return currentTest->equal(a, b);
}

static void test_free_key(gpointer data)
{
	currentTest->keyDestroyed.push_back(data);

	if(currentTest->keyDestroy != NULL) {
		currentTest->keyDestroy(data);
	}
}

static void test_free_value(gpointer data)
{
	currentTest->valueDestroyed.push_back(data);

	if(currentTest->valueDestroy != NULL) {
		currentTest->valueDestroy(data);
	}
}

static void test_foreach_clear()
{
	foreachCallbacks.clear();
	foreachLastUserData = NULL;
}

static void test_foreach_callback(gpointer key, gpointer value, gpointer userData)
{
	foreachCallbacks.push_back(std::make_pair(key, value));
	foreachLastUserData = userData;
}

static gboolean test_find_callback(gpointer key, gpointer value, gpointer userData)
{
	return strcmp((const char *) key, (const char *) userData) == 0;
}

TEST_F(GHashTableTest, insert)
{
	Create();
	const char *testKey = "foo";
	int testValue = 1337;

	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) testKey, &testValue);
	ASSERT_TRUE(inserted) << "inserted elemented should not have existed yet";
	ASSERT_EQ(hashed.size(), 1) << "inserted element should have been hashed";
	ASSERT_EQ(hashed[0], testKey) << "inserted element should have been hashed";
}

TEST_F(GHashTableTest, insertTwice)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "foo";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	ASSERT_FALSE(inserted) << "twice inserted elemented should already exist";
	ASSERT_EQ(equaled.size(), 1) << "inserted elements should have been equaled";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) testFirstKey, (gconstpointer) testSecondKey)) << "inserted elements should have been equaled";
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testSecondKey) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted first element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "inserted first element should have had its value destroyed";
}

TEST_F(GHashTableTest, insertReplace)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "foo";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	gboolean inserted = g_hash_table_replace(hashTable, (gpointer) testSecondKey, &testSecondValue);
	ASSERT_FALSE(inserted) << "twice inserted elemented should already exist";
	ASSERT_GE(equaled.size(), 1) << "inserted elements should have been equaled at least once";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) testFirstKey, (gconstpointer) testSecondKey)) << "inserted elements should have been equaled";
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted second element should have replaced key of first element";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testFirstKey) << "inserted second element should have replaced key of first element";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted first element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "inserted first element should have had its value destroyed";
}

TEST_F(GHashTableTest, contains)
{
	Create();
	const char *testKey = "foo";
	int testValue = 1337;

	gboolean contains = g_hash_table_contains(hashTable, (gpointer) testKey);
	ASSERT_FALSE(contains) << "hash table should not contain element before it is inserted";
	g_hash_table_insert(hashTable, (gpointer) testKey, &testValue);
	contains = g_hash_table_contains(hashTable, (gpointer) testKey);
	ASSERT_TRUE(contains) << "hash table should contain element after it was inserted";
}

TEST_F(GHashTableTest, size)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;
	
	guint size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 0) << "hash table should contain zero elements before an element is inserted";
	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testSecondKey);
	size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 1) << "hash table should contain one element after an element was inserted";
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 2) << "hash table should contain two elements after two element were inserted";
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 2) << "hash table should still contain two elements after an element was replaced";
}

TEST_F(GHashTableTest, lookup)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "foo";
	int testValue = 1337;

	gpointer value = g_hash_table_lookup(hashTable, testFirstKey);
	ASSERT_TRUE(value == NULL) << "hash table should fail to look up entry before it is inserted";
	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testValue);
	value = g_hash_table_lookup(hashTable, testSecondKey);
	ASSERT_TRUE(value == &testValue) << "hash table should look up entry after it was inserted";
	ASSERT_GE(equaled.size(), 1) << "inserted elements should have been equaled at least once";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) testFirstKey, (gconstpointer) testSecondKey)) << "inserted elements should have been equaled";
}

TEST_F(GHashTableTest, lookupExtended)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "foo";
	int testValue = 1337;

	gboolean found;
	gpointer origKey = NULL;
	gpointer value = NULL;
	found = g_hash_table_lookup_extended(hashTable, testFirstKey, &origKey, &value);
	ASSERT_FALSE(found) << "hash table should fail to look up entry before it is inserted";
	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testValue);
	found = g_hash_table_lookup_extended(hashTable, testSecondKey, &origKey, &value);
	ASSERT_TRUE(found) << "hash table should look up entry after it was inserted";
	ASSERT_EQ(testFirstKey, origKey) << "extended hash table lookup should return original key";
	ASSERT_EQ(&testValue, value) << "extended hash table lookup should return value";
	ASSERT_GE(equaled.size(), 1) << "inserted elements should have been equaled at least once";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) testFirstKey, (gconstpointer) testSecondKey)) << "inserted elements should have been equaled";
}

TEST_F(GHashTableTest, foreach)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;
	char testUserData = 'X';

	test_foreach_clear();
	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	g_hash_table_foreach(hashTable, test_foreach_callback, &testUserData);
	ASSERT_EQ(foreachCallbacks.size(), 2) << "foreach callback should have been called two times";
	ASSERT_NE(std::find(foreachCallbacks.begin(), foreachCallbacks.end(), std::make_pair((gpointer) testFirstKey, (gpointer) &testFirstValue)), foreachCallbacks.end()) << "foreach callback should be called with first element";
	ASSERT_NE(std::find(foreachCallbacks.begin(), foreachCallbacks.end(), std::make_pair((gpointer) testSecondKey, (gpointer) &testSecondValue)), foreachCallbacks.end()) << "foreach callback should be called with second element";
	ASSERT_EQ(foreachLastUserData, &testUserData) << "foreach callback should pass correct user data";
}

TEST_F(GHashTableTest, find)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	const char *testThirdKey = "baz";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	gpointer value = g_hash_table_find(hashTable, test_find_callback, (gpointer) testSecondKey);
	ASSERT_EQ(value, &testSecondValue) << "find should find existing key in hash table";
	value = g_hash_table_find(hashTable, test_find_callback, &testSecondKey);
	ASSERT_TRUE(value == NULL) << "find should not find non-existing key in hash table";
}

TEST_F(GHashTableTest, remove)
{
	Create();
	const char *testKey = "foo";
	int testValue = 1337;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testValue);
	gboolean removed = g_hash_table_remove(hashTable, testKey);
	ASSERT_TRUE(removed) << "removing existing element from hashtable should succeed";
	ASSERT_EQ(keyDestroyed.size(), 1) << "removed element key should be freed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testKey) << "removed element key should be freed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "removed element value should be freed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testValue) << "removed element value should be freed";
	removed = g_hash_table_remove(hashTable, testKey);
	ASSERT_FALSE(removed) << "removing non-existing element from hashtable should fail";
}

TEST_F(GHashTableTest, steal)
{
	Create();
	const char *testKey = "foo";
	int testValue = 1337;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testValue);
	gboolean removed = g_hash_table_steal(hashTable, testKey);
	ASSERT_TRUE(removed) << "stealing existing element from hashtable should succeed";
	ASSERT_EQ(keyDestroyed.size(), 0) << "stolen element key should not be freed";
	ASSERT_EQ(valueDestroyed.size(), 0) << "stolen element value should not be freed";
	removed = g_hash_table_steal(hashTable, testKey);
	ASSERT_FALSE(removed) << "stealing non-existing element from hashtable should fail";
}

TEST_F(GHashTableTest, foreachRemove)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	guint removed = g_hash_table_foreach_remove(hashTable, test_find_callback, (gpointer) testSecondKey);
	ASSERT_EQ(removed, 1) << "foreach remove should have removed one matching element";
	ASSERT_EQ(keyDestroyed.size(), 1) << "removed element key should be freed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testSecondKey) << "removed element key should be freed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "removed element value should be freed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testSecondValue) << "removed element value should be freed";
	removed = g_hash_table_foreach_remove(hashTable, test_find_callback, (gpointer) testSecondKey);
	ASSERT_EQ(removed, 0) << "foreach remove should be idempotent";
}

TEST_F(GHashTableTest, foreachSteal)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	guint removed = g_hash_table_foreach_steal(hashTable, test_find_callback, (gpointer) testSecondKey);
	ASSERT_EQ(removed, 1) << "foreach steal should have stolen one matching element";
	ASSERT_EQ(keyDestroyed.size(), 0) << "stolen element key should not be freed";
	ASSERT_EQ(valueDestroyed.size(), 0) << "stolen element value should not be freed";
	removed = g_hash_table_foreach_remove(hashTable, test_find_callback, (gpointer) testSecondKey);
	ASSERT_EQ(removed, 0) << "foreach steal should be idempotent";
}

TEST_F(GHashTableTest, removeAll)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	g_hash_table_remove_all(hashTable);
	ASSERT_EQ(keyDestroyed.size(), 2) << "removed element keys should be freed";
	ASSERT_NE(std::find(keyDestroyed.begin(), keyDestroyed.end(), (gpointer) testFirstKey), keyDestroyed.end()) << "first key should be freed";
	ASSERT_NE(std::find(keyDestroyed.begin(), keyDestroyed.end(), (gpointer) testSecondKey), keyDestroyed.end()) << "second key should be freed";
	ASSERT_EQ(valueDestroyed.size(), 2) << "removed element values should be freed";
	ASSERT_NE(std::find(valueDestroyed.begin(), valueDestroyed.end(), &testFirstValue), valueDestroyed.end()) << "first value should be freed";
	ASSERT_NE(std::find(valueDestroyed.begin(), valueDestroyed.end(), &testSecondValue), valueDestroyed.end()) << "second value should be freed";
	guint size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 0) << "removing all elements should leave hash table empty";
}

TEST_F(GHashTableTest, stealAll)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);
	g_hash_table_steal_all(hashTable);
	ASSERT_EQ(keyDestroyed.size(), 0) << "stolen element keys should not be freed";
	ASSERT_EQ(valueDestroyed.size(), 0) << "stolen element values should not be freed";
	guint size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 0) << "stealing all elements should leave hash table empty";
}

TEST_F(GHashTableTest, freeInserted)
{
	Create();
	const char *testKey = "foo";
	int testValue = 1337;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testValue);
	ASSERT_EQ(keyDestroyed.size(), 0) << "inserted element should not have its key destroyed yet";
	ASSERT_EQ(valueDestroyed.size(), 0) << "inserted element should not have its value destroyed yet";

	Destroy();
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testKey) << "inserted element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testValue) << "inserted element should have had its value destroyed";
}

TEST_F(GHashTableTest, getKeys)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);

	GList *keysList = g_hash_table_get_keys(hashTable);
	ASSERT_EQ(2, g_list_length(keysList)) << "keys array should have size 2";
	GList *firstKeyElement = g_list_find(keysList, (gpointer) testFirstKey);
	ASSERT_TRUE(firstKeyElement != NULL) << "first key should be in keys list";
	GList *secondKeyElement = g_list_find(keysList, (gpointer) testSecondKey);
	ASSERT_TRUE(secondKeyElement != NULL) << "second key should be in keys list";
	g_list_free(keysList);
}

TEST_F(GHashTableTest, getValues)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);

	GList *valuesList = g_hash_table_get_values(hashTable);
	ASSERT_EQ(2, g_list_length(valuesList)) << "values array should have size 2";
	GList *firstKeyElement = g_list_find(valuesList, &testFirstValue);
	ASSERT_TRUE(firstKeyElement != NULL) << "first value should be in values list";
	GList *secondKeyElement = g_list_find(valuesList, &testSecondValue);
	ASSERT_TRUE(secondKeyElement != NULL) << "second value should be in values list";
	g_list_free(valuesList);
}

TEST_F(GHashTableTest, getKeysAsArray)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer) testSecondKey, &testSecondValue);

	guint length;
	gpointer *array = g_hash_table_get_keys_as_array(hashTable, &length);
	ASSERT_EQ(length, 2) << "keys array should have size 2";
	if(array[0] == testFirstKey) {
		ASSERT_EQ(array[1], testSecondKey) << "array should contain keys";
	} else {
		ASSERT_EQ(array[0], testSecondKey) << "array should contain keys";
		ASSERT_EQ(array[1], testFirstKey) << "array should contain keys";
	}
	ASSERT_TRUE(array[2] == NULL) << "array should be NULL terminated";
	g_free(array);
}

TEST_F(GHashTableTest, refUnref)
{
	Create();
	const char *testKey = "foo";
	int testValue = 1337;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testValue);
	ASSERT_EQ(keyDestroyed.size(), 0) << "inserted element should not have its key destroyed yet";
	ASSERT_EQ(valueDestroyed.size(), 0) << "inserted element should not have its value destroyed yet";

	g_hash_table_ref(hashTable);
	ASSERT_EQ(keyDestroyed.size(), 0) << "inserted element should not have its key destroyed yet";
	ASSERT_EQ(valueDestroyed.size(), 0) << "inserted element should not have its value destroyed yet";
	g_hash_table_unref(hashTable);
	ASSERT_EQ(keyDestroyed.size(), 0) << "inserted element should not have its key destroyed yet";
	ASSERT_EQ(valueDestroyed.size(), 0) << "inserted element should not have its value destroyed yet";
	g_hash_table_unref(hashTable);
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testKey) << "inserted element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testValue) << "inserted element should have had its value destroyed";
	created = false;
}

TEST_F(GHashTableTest, iter)
{
	Create();
	const char *testFirstKey = "foo";
	const char *testSecondKey = "bar";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer)testFirstKey, &testFirstValue);
	g_hash_table_insert(hashTable, (gpointer)testSecondKey, &testSecondValue);

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	std::vector< std::pair<gpointer, gpointer> > iteratedElements;
	g_hash_table_iter_init(&iter, hashTable);
	while(g_hash_table_iter_next(&iter, &key, &value)) {
		iteratedElements.push_back(std::make_pair(key, value));
	}

	ASSERT_EQ(iteratedElements.size(), 2) << "iterator should have iterated over two elements";
	ASSERT_NE(std::find(iteratedElements.begin(), iteratedElements.end(), std::make_pair((gpointer) testFirstKey, (gpointer) &testFirstValue)), iteratedElements.end()) << "first element should have been iterated over";
	ASSERT_NE(std::find(iteratedElements.begin(), iteratedElements.end(), std::make_pair((gpointer) testSecondKey, (gpointer) &testSecondValue)), iteratedElements.end()) << "second element should have been iterated over";
}

TEST_F(GHashTableTest, iterGetHashTable)
{
	Create();

	GHashTableIter iter;
	g_hash_table_iter_init(&iter, hashTable);
	GHashTable *extractedHashTable = g_hash_table_iter_get_hash_table(&iter);
	ASSERT_EQ(extractedHashTable, hashTable) << "hash table extracted from iterator should match hash table the iterator was created from";
}

TEST_F(GHashTableTest, iterReplace)
{
	Create();
	const char *testKey = "foo";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testFirstValue);

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init(&iter, hashTable);
	gboolean hasNext = g_hash_table_iter_next(&iter, &key, &value);
	ASSERT_TRUE(hasNext) << "hash table iterator should find first element";
	ASSERT_EQ(valueDestroyed.size(), 0) << "no value should be freed yet before replacing";
	g_hash_table_iter_replace(&iter, &testSecondValue);
	ASSERT_EQ(valueDestroyed.size(), 1) << "replaced element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "replaced element should have had its value destroyed";
	hasNext = g_hash_table_iter_next(&iter, &key, &value);
	ASSERT_FALSE(hasNext) << "hash table should have no more next element";
}

TEST_F(GHashTableTest, iterRemove)
{
	Create();
	const char *testKey = "foo";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testFirstValue);

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init(&iter, hashTable);
	gboolean hasNext = g_hash_table_iter_next(&iter, &key, &value);
	ASSERT_TRUE(hasNext) << "hash table iterator should find first element";
	ASSERT_EQ(keyDestroyed.size(), 0) << "element should not have its key freed yet before removing it";
	ASSERT_EQ(valueDestroyed.size(), 0) << "element should not have its value freed yet before removing it";
	g_hash_table_iter_remove(&iter);
	ASSERT_EQ(keyDestroyed.size(), 1) << "removed element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) testKey) << "removed element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "removed element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "replaced element should have had its value destroyed";
	hasNext = g_hash_table_iter_next(&iter, &key, &value);
	ASSERT_FALSE(hasNext) << "hash table should have no more next element";
	guint size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 0) << "removing only element should leave hash table empty";
}

TEST_F(GHashTableTest, iterSteal)
{
	Create();
	const char *testKey = "foo";
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) testKey, &testFirstValue);

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init(&iter, hashTable);
	gboolean hasNext = g_hash_table_iter_next(&iter, &key, &value);
	ASSERT_TRUE(hasNext) << "hash table iterator should find first element";
	g_hash_table_iter_steal(&iter);
	ASSERT_EQ(keyDestroyed.size(), 0) << "stealing element should not cause its key to be destroyed";
	ASSERT_EQ(valueDestroyed.size(), 0) << "stealing element should not cause its value to be destroyed";
	hasNext = g_hash_table_iter_next(&iter, &key, &value);
	ASSERT_FALSE(hasNext) << "hash table should have no more next element";
	guint size = g_hash_table_size(hashTable);
	ASSERT_EQ(size, 0) << "stealing only element should leave hash table empty";
}

TEST_F(GHashTableTest, directInsert)
{
	hash = g_direct_hash;
	equal = g_direct_equal;

	Create();
	int testKey = 42;
	int testValue = 1337;

	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testKey, &testValue);
	ASSERT_TRUE(inserted) << "inserted elemented should not have existed yet";
	ASSERT_EQ(hashed.size(), 1) << "inserted element should have been hashed";
	ASSERT_EQ(hashed[0], &testKey) << "inserted element should have been hashed";
}

TEST_F(GHashTableTest, directInsertTwice)
{
	hash = g_direct_hash;
	equal = g_direct_equal;

	Create();
	int testKey = 27;
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) &testKey, &testFirstValue);
	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testKey, &testSecondValue);
	ASSERT_FALSE(inserted) << "twice inserted elemented should already exist";
	ASSERT_EQ(equaled.size(), 1) << "inserted elements should have been equaled";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) &testKey, (gconstpointer) &testKey)) << "inserted elements should have been equaled";
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) &testKey) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted first element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "inserted first element should have had its value destroyed";
}

TEST_F(GHashTableTest, intInsert)
{
	hash = g_int_hash;
	equal = g_int_equal;

	Create();
	int testKey = 42;
	int testValue = 1337;

	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testKey, &testValue);
	ASSERT_TRUE(inserted) << "inserted elemented should not have existed yet";
	ASSERT_EQ(hashed.size(), 1) << "inserted element should have been hashed";
	ASSERT_EQ(hashed[0], &testKey) << "inserted element should have been hashed";
}

TEST_F(GHashTableTest, intInsertTwice)
{
	hash = g_int_hash;
	equal = g_int_equal;

	Create();
	int testFirstKey = 27;
	int testSecondKey = 27;
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) &testFirstKey, &testFirstValue);
	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testSecondKey, &testSecondValue);
	ASSERT_FALSE(inserted) << "twice inserted elemented should already exist";
	ASSERT_EQ(equaled.size(), 1) << "inserted elements should have been equaled";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) &testFirstKey, (gconstpointer) &testSecondKey)) << "inserted elements should have been equaled";
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) &testSecondKey) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted first element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "inserted first element should have had its value destroyed";
}

TEST_F(GHashTableTest, int64Insert)
{
	hash = g_int64_hash;
	equal = g_int64_equal;

	Create();
	gint64 testKey = 42LL;
	int testValue = 1337;

	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testKey, &testValue);
	ASSERT_TRUE(inserted) << "inserted elemented should not have existed yet";
	ASSERT_EQ(hashed.size(), 1) << "inserted element should have been hashed";
	ASSERT_EQ(hashed[0], &testKey) << "inserted element should have been hashed";
}

TEST_F(GHashTableTest, int64InsertTwice)
{
	hash = g_int64_hash;
	equal = g_int64_equal;

	Create();
	gint64 testFirstKey = 27LL;
	gint64 testSecondKey = 27LL;
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) &testFirstKey, &testFirstValue);
	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testSecondKey, &testSecondValue);
	ASSERT_FALSE(inserted) << "twice inserted elemented should already exist";
	ASSERT_EQ(equaled.size(), 1) << "inserted elements should have been equaled";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) &testFirstKey, (gconstpointer) &testSecondKey)) << "inserted elements should have been equaled";
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) &testSecondKey) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted first element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "inserted first element should have had its value destroyed";
}

TEST_F(GHashTableTest, doubleInsert)
{
	hash = g_double_hash;
	equal = g_double_equal;

	Create();
	double testKey = 0.5;
	int testValue = 1337;

	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testKey, &testValue);
	ASSERT_TRUE(inserted) << "inserted elemented should not have existed yet";
	ASSERT_EQ(hashed.size(), 1) << "inserted element should have been hashed";
	ASSERT_EQ(hashed[0], &testKey) << "inserted element should have been hashed";
}

TEST_F(GHashTableTest, doubleInsertTwice)
{
	hash = g_double_hash;
	equal = g_double_equal;

	Create();
	double testFirstKey = 0.5;
	double testSecondKey = 0.5;
	int testFirstValue = 1337;
	int testSecondValue = 42;

	g_hash_table_insert(hashTable, (gpointer) &testFirstKey, &testFirstValue);
	gboolean inserted = g_hash_table_insert(hashTable, (gpointer) &testSecondKey, &testSecondValue);
	ASSERT_FALSE(inserted) << "twice inserted elemented should already exist";
	ASSERT_EQ(equaled.size(), 1) << "inserted elements should have been equaled";
	ASSERT_EQ(equaled[0], make_ordered_pair((gconstpointer) &testFirstKey, (gconstpointer) &testSecondKey)) << "inserted elements should have been equaled";
	ASSERT_EQ(keyDestroyed.size(), 1) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(keyDestroyed[0], (gpointer) &testSecondKey) << "inserted second element should have had its key destroyed";
	ASSERT_EQ(valueDestroyed.size(), 1) << "inserted first element should have had its value destroyed";
	ASSERT_EQ(valueDestroyed[0], (gpointer) &testFirstValue) << "inserted first element should have had its value destroyed";
}
