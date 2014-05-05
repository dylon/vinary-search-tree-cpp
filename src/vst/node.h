#ifndef __VST_NODE_H__
#define __VST_NODE_H__

#include <vector>

namespace vst {

template <class NodeType, class KeyType, class ValueType>
class Node {
public:

  Node() {
    // empty constructor
  }

  virtual ~Node() {
    if (lesser_child) delete lesser_child;
    if (greater_child) delete greater_child;
  }

  inline NodeType* setKey(KeyType const key) {
    this->key = key;
    return static_cast<NodeType*>(this);
  }

  inline KeyType getKey() const {
    return key;
  }

  inline NodeType* addValue(ValueType const value) {
    values.push_back(value);
    return static_cast<NodeType*>(this);
  }

  inline const ::std::vector<ValueType>& getValues() const {
    return values;
  }

  inline ValueType getValue() const {
    return values.front();
  }

  inline NodeType* setGreaterChild(
      NodeType* const greater_child) {
    this->greater_child = greater_child;
    return static_cast<NodeType*>(this);
  }

  inline NodeType* getGreaterChild() const {
    return greater_child;
  }

  inline NodeType* setLesserChild(
      NodeType* const lesser_child) {
    this->lesser_child = lesser_child;
    return static_cast<NodeType*>(this);
  }

  inline NodeType* getLesserChild() const {
    return lesser_child;
  }

  inline NodeType* setGreaterNeighbor(
      NodeType* const greater_neighbor) {
    this->greater_neighbor = greater_neighbor;
    return static_cast<NodeType*>(this);
  }

  inline NodeType* getGreaterNeighbor() const {
    return greater_neighbor;
  }

  inline NodeType* setLesserNeighbor(
      NodeType* const lesser_neighbor) {
    this->lesser_neighbor = lesser_neighbor;
    return static_cast<NodeType*>(this);
  }

  inline NodeType* getLesserNeighbor() const {
    return lesser_neighbor;
  }

  inline bool isLeaf() const {
    return !lesser_child && !greater_child;
  }

  inline bool isBranch() const {
    return !lesser_child != !greater_child;
  }

  int getMaxChildHeight() const {
    int const lesser_child_height = (lesser_child)
      ? lesser_child->getHeight()
      : -1;
    int const greater_child_height = (greater_child)
      ? greater_child->getHeight()
      : -1;
    return (lesser_child_height > greater_child_height)
      ? lesser_child_height
      : greater_child_height;
  }

  inline NodeType* setHeight(int const height) {
    this->height = height;
    return static_cast<NodeType*>(this);
  }

  inline int getHeight() const {
    return height;
  }

protected:
  KeyType key;
  std::vector<ValueType> values;
  NodeType* greater_child = nullptr;
  NodeType* lesser_child = nullptr;
  NodeType* greater_neighbor = nullptr;
  NodeType* lesser_neighbor = nullptr;
  int height = 0;
};

}

#endif
