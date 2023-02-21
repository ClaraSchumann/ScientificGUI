#include <gtest/gtest.h>

TEST(demo, demo) { ASSERT_EQ(1, 1); }

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  // ::testing::internal::CaptureStdout();
  // std::cout << ::testing::internal::GetCapturedStdout();

  return RUN_ALL_TESTS();
}

