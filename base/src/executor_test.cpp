#include <cmath>
#include <iostream>
#include <type_traits>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/executor.h"
}

class ShovelerExecutorTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		executor = shovelerExecutorCreateDirect();
		callbackCalled = false;
	}

	virtual void TearDown()
	{
		shovelerExecutorFree(executor);
	}

	static void testCallback(void *userData)
	{
		ShovelerExecutorTest *test = (ShovelerExecutorTest *) userData;
		test->callbackCalled = true;
	}

	ShovelerExecutor *executor;
	bool callbackCalled;
};

TEST_F(ShovelerExecutorTest, scheduleNow)
{
	shovelerExecutorSchedule(executor, 0, testCallback, this);
	ASSERT_FALSE(callbackCalled) << "callback should not be called yet after schedule";

	shovelerExecutorUpdateNow(executor);
	ASSERT_TRUE(callbackCalled) << "callback must have been executed";
}

TEST_F(ShovelerExecutorTest, scheduleFar)
{
	shovelerExecutorSchedule(executor, 60000, testCallback, this);
	ASSERT_FALSE(callbackCalled) << "callback should not be called yet after schedule";

	shovelerExecutorUpdateNow(executor);
	ASSERT_FALSE(callbackCalled) << "callback should still not have been called";
}

TEST_F(ShovelerExecutorTest, scheduleDelay)
{
	shovelerExecutorSchedule(executor, 1, testCallback, this);
	ASSERT_FALSE(callbackCalled) << "callback should not be called yet after schedule";

	shovelerExecutorUpdate(executor, 999);
	ASSERT_FALSE(callbackCalled) << "callback should still not have been called";

	shovelerExecutorUpdate(executor, 1);
	ASSERT_TRUE(callbackCalled) << "callback must have been executed";
}

TEST_F(ShovelerExecutorTest, schedulePeriodic)
{
	shovelerExecutorSchedulePeriodic(executor, 0, 1, testCallback, this);
	ASSERT_FALSE(callbackCalled) << "callback should not be called yet after schedule";

	shovelerExecutorUpdate(executor, 0);
	ASSERT_TRUE(callbackCalled) << "callback must have been executed";

	callbackCalled = false;
	shovelerExecutorUpdate(executor, 999);
	ASSERT_FALSE(callbackCalled) << "callback should not have been called yet a second time";

	shovelerExecutorUpdate(executor, 1);
	ASSERT_TRUE(callbackCalled) << "callback must have been executed a second time";
}

TEST_F(ShovelerExecutorTest, scheduleRemove)
{
	ShovelerExecutorCallback *callback = shovelerExecutorSchedule(executor, 1, testCallback, this);
	ASSERT_FALSE(callbackCalled) << "callback should not be called directly after schedule";

	shovelerExecutorUpdate(executor, 0);
	ASSERT_FALSE(callbackCalled) << "callback should not have been executed";

	bool removed = shovelerExecutorRemoveCallback(executor, callback);
	ASSERT_TRUE(removed) << "callback should have been removed successfully";

	shovelerExecutorUpdate(executor, 1000);
	ASSERT_FALSE(callbackCalled) << "callback should still not have been called";
}
