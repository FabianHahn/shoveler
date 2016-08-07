#include <gtest/gtest.h>
#include <glib.h>

class GDateTimeTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
		dateTime = g_date_time_new_now_local();
	}

	virtual void TearDown()
	{
		if(dateTime != NULL) {
			g_date_time_unref(dateTime);
		}
	}

	GDateTime *dateTime;
};

TEST_F(GDateTimeTest, getYear)
{
	gint year = g_date_time_get_year(dateTime);
	ASSERT_GE(year, 2016) << "current year should be at least the year this test was written";
}

TEST_F(GDateTimeTest, getMonth)
{
	gint month = g_date_time_get_month(dateTime);
	ASSERT_GE(month, 1) << "current month should be at least January";
	ASSERT_LE(month, 12) << "current month should be at most December";
}

TEST_F(GDateTimeTest, getDayOfMonth)
{
	gint dayOfMonth = g_date_time_get_day_of_month(dateTime);
	ASSERT_GE(dayOfMonth, 1) << "current day of month should be at least 1";
	ASSERT_LE(dayOfMonth, 31) << "current day of month should be at most 31";
}

TEST_F(GDateTimeTest, getHour)
{
	gint hour = g_date_time_get_hour(dateTime);
	ASSERT_GE(hour, 0) << "current hour should be at least 0";
	ASSERT_LE(hour, 23) << "current hour should be at most 31";
}

TEST_F(GDateTimeTest, getMinute)
{
	gint minute = g_date_time_get_minute(dateTime);
	ASSERT_GE(minute, 0) << "current minute should be at least 0";
	ASSERT_LE(minute, 59) << "current minute should be at most 59";
}

TEST_F(GDateTimeTest, getSecond)
{
	gint second = g_date_time_get_second(dateTime);
	ASSERT_GE(second, 0) << "current second should be at least 0";
	ASSERT_LE(second, 59) << "current second should be at most 59";
}
