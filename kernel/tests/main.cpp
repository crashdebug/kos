#include <gtest/gtest.h>
#include "terminal_tests.cpp"
#include "memory_tests.cpp"
#include "vector_tests.cpp"
#include "command_tests.cpp"
#include <cstdio>
#include "../../libc/include/stdio.h"

void putchr(char c)
{
  std::cout << c;
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	kos::init_printf(&putchr);
	return RUN_ALL_TESTS();
}
