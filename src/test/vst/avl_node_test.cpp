#include <gtest/gtest.h>

#include "../../vst/avl_node.h"

using namespace vst;

class AvlNodeTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    parent = new AvlNode<int,int>();
    node = new AvlNode<int,int>();
    lesser_child = new AvlNode<int,int>();
    greater_child = new AvlNode<int,int>();

    parent->setKey(4)->addValue(0);
    parent->setLesserChild(node);
    parent->setLesserNeighbor(greater_child);

    node->setKey(2)->addValue(2);
    node->setParent(parent);
    node->setLesserChild(lesser_child);
    node->setGreaterChild(greater_child);
    node->setLesserNeighbor(lesser_child);
    node->setGreaterNeighbor(greater_child);

    lesser_child->setKey(1)->addValue(1);
    lesser_child->setParent(node);
    lesser_child->setGreaterNeighbor(node);

    greater_child->setKey(3)->addValue(3);
    greater_child->setParent(node);
    greater_child->setLesserNeighbor(node);
    greater_child->setGreaterNeighbor(parent);
  }

  virtual void TearDown() {
    delete parent;
  }

  AvlNode<int,int>* node;
  AvlNode<int,int>* parent;
  AvlNode<int,int>* lesser_child;
  AvlNode<int,int>* greater_child;
};

TEST(NewAvlNodeTest, NodeReferencesShouldBeNull) {
  AvlNode<int,int> node;
  ASSERT_EQ(nullptr, node.getLesserChild());
  ASSERT_EQ(nullptr, node.getGreaterChild());
  ASSERT_EQ(nullptr, node.getLesserNeighbor());
  ASSERT_EQ(nullptr, node.getGreaterNeighbor());
  ASSERT_EQ(nullptr, node.getParent());
}

TEST_F(AvlNodeTest, TestMaxChildHeight) {
  lesser_child->setHeight(2);
  greater_child->setHeight(1);
  ASSERT_EQ(2, node->getMaxChildHeight());

  lesser_child->setHeight(0);
  greater_child->setHeight(3);
  ASSERT_EQ(3, node->getMaxChildHeight());
}

TEST_F(AvlNodeTest, TestBalance) {
  lesser_child->setHeight(4);
  greater_child->setHeight(1);
  ASSERT_FALSE(node->isBalanced());

  lesser_child->setHeight(2);
  greater_child->setHeight(2);
  ASSERT_TRUE(node->isBalanced());
}

TEST_F(AvlNodeTest, TestLeavesAndBranches) {
  ASSERT_FALSE(parent->isLeaf());
  ASSERT_TRUE(parent->isBranch());
  ASSERT_FALSE(node->isLeaf());
  ASSERT_FALSE(node->isBranch());
  ASSERT_TRUE(lesser_child->isLeaf());
  ASSERT_FALSE(lesser_child->isBranch());
  ASSERT_TRUE(greater_child->isLeaf());
  ASSERT_FALSE(greater_child->isBranch());
}
