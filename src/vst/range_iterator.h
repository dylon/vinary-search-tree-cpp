#ifndef __VST_RANGE_ITERATOR_H__
#define __VST_RANGE_ITERATOR_H__

#include <functional>

#include "iterator.h"
#include "node.h"

namespace vst {

template <class NodeType, class KeyType>
class RangeIterator : public Iterator<NodeType*> {
public:
  using Iterator<NodeType*>::Iterator;

  inline RangeIterator* setNode(NodeType* const node) {
    this->node = node;
    return this;
  }

  inline RangeIterator* setCompare(
      std::function<int (KeyType, KeyType)> const compare) {
    this->compare = compare;
    return this;
  }

  inline RangeIterator* setUpperKey(KeyType const upper_key) {
    this->upper_key = upper_key;
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && node) {
      if (compare(node->getKey(), upper_key) <= 0) {
        this->has_advanced = false;
        this->next_element = node;
        node = node->getGreaterNeighbor();
      }
    }
  }

private:
  NodeType* node = nullptr;
  std::function<int (KeyType, KeyType)> compare = {};
  KeyType upper_key = {};
};

}

#endif
