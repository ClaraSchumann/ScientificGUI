#include <gtest/gtest.h>

#include "SmartArray.hpp"

// #include "InMemoryBinaryPool.h"
// #include "InterprocessSingleton.hpp"

TEST(SmartArray, base)
{
  SmartArray<char[64], 5> t;

  t.get();
  t.get();
  t.get();
  t.get();
  ASSERT_EQ(t.get_chained_free_blocks_array(), std::vector<int>({4}));
  ASSERT_EQ(t.get_blocks_in_use_array(), std::vector<int>({0,1,2,3}));


  t.get();
  ASSERT_EQ(t.get_chained_free_blocks_array(), std::vector<int>({}));
  ASSERT_EQ(t.get_blocks_in_use_array(), std::vector<int>({0,1,2,3,4}));

  t.release(3);
  t.release(2);
  ASSERT_EQ(t.get_chained_free_blocks_array(), std::vector<int>({3,2}));
  ASSERT_EQ(t.get_blocks_in_use_array(), std::vector<int>({0,1,4}));

  t.release(4);
  t.release(1);
  t.release(0);
  ASSERT_EQ(t.get_chained_free_blocks_array(), std::vector<int>({3,2,4,1,0}));
  ASSERT_EQ(t.get_blocks_in_use_array(), std::vector<int>({}));

  t.get();
  ASSERT_EQ(t.get_chained_free_blocks_array(), std::vector<int>({2,4,1,0}));
  ASSERT_EQ(t.get_blocks_in_use_array(), std::vector<int>({3}));
}

TEST(InMemoryBinary, PoolBase){

};
