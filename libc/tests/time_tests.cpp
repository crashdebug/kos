#include <gtest/gtest.h>
#include "../include/time.h"

extern "C" time_t ticks()
{
	return 0;
}

TEST(TimeTests, mktimeTest)
{
	kos::tm t;
	t.tm_sec = 1;
	t.tm_min = 0;
	t.tm_hour = 0;
	t.tm_mday = 1;
	t.tm_mon = 0;
	t.tm_year = 0;
	ASSERT_EQ(TICKS_PER_SECOND, kos::mktime(&t));

	t.tm_sec = 0;
	t.tm_min = 1;
	ASSERT_EQ(TICKS_PER_MINUTE, kos::mktime(&t));

	t.tm_year = 2016;
	t.tm_mon = 0;
	t.tm_mday = 1;
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 1;
	ASSERT_TRUE(kos::mktime(&t) % 1000 == 0);
}
