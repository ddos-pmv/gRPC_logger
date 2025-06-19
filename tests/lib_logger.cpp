#include <gtest/gtest.h>
#include <ring_buffer.h>

struct Dummy {};

int main() {
  ::testing::InitGoogleTest();

  return RUN_ALL_TESTS();
}