#include <iostream>

#include <gtest/gtest.h>
#include <glib.h>

class GTimeTest : public ::testing::Test {
public:
	virtual void SetUp()
	{

	}

	virtual void TearDown()
	{

	}
};

TEST_F(GTimeTest, getMonotonicTime)
{
	// cannot really assert on value, but we can at least test executing without crashing
	gint64 monotonicTime = g_get_monotonic_time();
	std::cout << "monotonic time: " << monotonicTime << std::endl;
}

TEST_F(GTimeTest, getRealTime)
{
	// cannot really assert on value, but we can at least test executing without crashing
	gint64 realTime = g_get_real_time();
	std::cout << "real time: " << realTime << std::endl;
}
