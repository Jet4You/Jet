#include <gtest/gtest.h>

TEST(Addition, Basic) {
  EXPECT_EQ(1 + 1, 2);
}

auto main(int argc, char* argv[]) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}