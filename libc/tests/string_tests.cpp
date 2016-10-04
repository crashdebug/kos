#include <gtest/gtest.h>
#include "../include/string.h"

TEST(StringTests, StrcmpTest)
{
	ASSERT_EQ(0, kos::strcmp("", ""));
	ASSERT_EQ(-1, kos::strcmp("a", "b"));
	ASSERT_EQ(1, kos::strcmp("b", "a"));
	ASSERT_EQ(0, kos::strcmp("a", "a"));
}
