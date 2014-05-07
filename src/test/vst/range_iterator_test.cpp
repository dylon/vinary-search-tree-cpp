#include <functional>

#include <gtest/gtest.h>

#include "../../vst/avl_node.h"
#include "../../vst/range_iterator.h"

using namespace vst;

class RangeIteratorTest : public ::testing::Test {
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

  std::function<int (int,int)> compare = [](int const a, int const b) {
    return a - b;
  };
};

TEST_F(RangeIteratorTest, TestSequence) {
  RangeIterator<AvlNode<int,int>,int>* iter;

  iter = new RangeIterator<AvlNode<int,int>,int>();
  iter->setNode(first)->setCompare(compare)->setUpperKey(100);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(first, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(second, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(third, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(fourth, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(fifth, iter->next());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = new RangeIterator<AvlNode<int,int>,int>();
  iter->setNode(second)->setCompare(compare)->setUpperKey(4);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(second, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(third, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(fourth, iter->next());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}
