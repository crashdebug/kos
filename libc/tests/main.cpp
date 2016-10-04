#include <gtest/gtest.h>
#include "printf_tests.cpp"
#include "string_tests.cpp"
#include "time_tests.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
