#include <gtest/gtest.h>
#include <vector.h>

TEST(VectorTests, VectorPushBackTest)
{
	kos::vector<int> v;
	for (int i = 1; i < 16; i++)
	{
		v.push_back(i);
		ASSERT_EQ(i, v.size());
		ASSERT_EQ(i, v.at(i - 1));
	}
}
