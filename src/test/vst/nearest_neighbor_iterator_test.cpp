#include <functional>

#include <gtest/gtest.h>

#include "../../vst/avl_node.h"
#include "../../vst/nearest_neighbor_iterator.h"

using namespace vst;

class NearestNeighborIteratorTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    first = new AvlNode<int,int>();
    second = new AvlNode<int,int>();
    third = new AvlNode<int,int>();
    fourth = new AvlNode<int,int>();
    fifth = new AvlNode<int,int>();

    first->setKey(1);
    second->setKey(2);
    third->setKey(3);
    fourth->setKey(4);
    fifth->setKey(5);

    first->setGreaterNeighbor(second);
    second->setLesserNeighbor(first)->setGreaterNeighbor(third);
    third->setLesserNeighbor(second)->setGreaterNeighbor(fourth);
    fourth->setLesserNeighbor(third)->setGreaterNeighbor(fifth);
    fifth->setLesserNeighbor(fourth);
  }

  virtual void TearDown() {
    delete first;
    delete second;
    delete third;
    delete fourth;
    delete fifth;
  }

  AvlNode<int,int>* first;
  AvlNode<int,int>* second;
  AvlNode<int,int>* third;
  AvlNode<int,int>* fourth;
  AvlNode<int,int>* fifth;

  std::function<double (int,int)> distance = [](int const a, int const b) {
    return (a > b) ? a - b : b - a;
  };
};

TEST_F(NearestNeighborIteratorTest, TestNearestNeighbor) {
  NearestNeighborIterator<AvlNode<int,int>, int>* iter;

  iter = new NearestNeighborIterator<AvlNode<int,int>, int>();
  iter->setKey(3)->setDistance(distance)->setLimit(3)->setNode(fourth);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(third, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(fourth, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(second, iter->next());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}
