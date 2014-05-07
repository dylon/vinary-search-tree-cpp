#include <gtest/gtest.h>

#include "vst/avl_node_test.cpp"
#include "vst/iterator_test.cpp"
#include "vst/range_iterator_test.cpp"
#include "vst/nearest_neighbor_iterator_test.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
