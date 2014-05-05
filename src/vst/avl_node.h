#ifndef __VST_AVL_NODE__
#define __VST_AVL_NODE__

#include "node.h"

namespace vst {

template <class KeyType, class ValueType>
class AvlNode : public Node<AvlNode<KeyType, ValueType>, KeyType, ValueType> {
public:

  AvlNode() : Node<AvlNode<KeyType, ValueType>, KeyType, ValueType>() {
    // empty constructor
  }

  ~AvlNode() {
    // empty destructor
  }

  inline AvlNode<KeyType, ValueType>* setParent(
      AvlNode<KeyType, ValueType>* const parent) {
    this->parent = parent;
    return this;
  }

  inline AvlNode<KeyType, ValueType>* getParent() const {
    return parent;
  }

  int getBalance() const {
    int const lesser_child_height = (this->lesser_child)
      ? this->lesser_child->getHeight()
      : -1;
    int const greater_child_height = (this->greater_child)
      ? this->greater_child->getHeight()
      : -1;
    return lesser_child_height - greater_child_height;
  }

  inline bool isBalanced() const {
    int const balance = getBalance();
    return -1 <= balance && balance <= 1;
  }

private:
  AvlNode<KeyType, ValueType>* parent = nullptr;
};

}

#endif
