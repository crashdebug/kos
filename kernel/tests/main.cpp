#include <gtest/gtest.h>
#include "terminal_tests.cpp"
#include "memory_tests.cpp"
#include "vector_tests.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
