#include <gtest/gtest.h>
//#include "../src/terminal.cpp"
//#include <cstdio>
#include <terminal.h>

TEST(TerminalTests, TerminalOutputTest)
{
	size_t width = 10;
	size_t height = 10;
	uint16_t* buffer = new uint16_t[width * height];

	Terminal::initialize(buffer, width, height);
//	kos::printf("%i", 1);
//	kos::printf("%i", 2);
//	kos::printf("%i", 3);

//	ASSERT_EQ(0x731, buffer[0]);
//	ASSERT_EQ(0x732, buffer[1]);
//	ASSERT_EQ(0x733, buffer[2]);

	delete[] buffer;
}
