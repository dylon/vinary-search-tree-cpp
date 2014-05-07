#ifndef __VST_TREE_H__
#define __VST_TREE_H__

#include <functional>
#include <vector>

#include "nearest_neighbor_iterator.h"
#include "range_iterator.h"

namespace vst {

template <class NodeType, class KeyType, class ValueType>
class Tree {
public:

  Tree() {
    // empty constructor
  }

  virtual ~Tree() {
    delete root;
  }

  inline unsigned int getSize() const {
    return size;
  }

  inline unsigned int getHeight() const {
    return (root != nullptr) ? root->getHeight() : 0;
  }

  inline NodeType* getGreatest() const {
    return (root != nullptr)
      ? getGreatest(root)
      : nullptr;
  }

  NodeType* getGreatest(NodeType* node) const {
    while (node->getGreaterChild()) {
      node = node->getGreaterChild();
    }
    return node;
  }

  inline NodeType* getLeast() const {
    return (root != nullptr)
      ? getLeast(root)
      : nullptr;
  }

  NodeType* getLeast(NodeType* node) const {
    if (!node) return nullptr;
    while (node->getLesserChild()) {
      node = node->getLesserChild();
    }
    return node;
  }

  bool tryInsert(KeyType const key, ValueType const value) {
    if (root == nullptr) {
      root = buildNode(key, value);
      size += 1;
      return true;
    }

    if (!containsKey(key)) {
      addDescendant(root, buildNode(key, value));
      size += 1;
      return true;
    }

    return false;
  }

  auto insert(KeyType const key, ValueType const value) {
    if (root == nullptr) {
      root = buildNode(key, value);
    }
    else if (NodeType* const node = find(key)) {
      node->addValue(value);
    }
    else {
      addDescendant(root, buildNode(key, value));
    }
    size += 1;
    return this;
  }

  inline bool containsKey(KeyType const key) const {
    return nullptr != find(key);
  }

  NodeType* find(KeyType const key) const {
    NodeType* node = root;
    while (node) {
      int const comparison = compare(key, node->getKey());
      if (comparison > 0) {
        node = node->getGreaterChild();
      }
      else if (comparison < 0) {
        node = node->getLesserChild();
      }
      else {
        break;
      }
    }
    return node;
  }

  NodeType* findNearest(KeyType const key) const {
    NodeType* node = root;
    while (node) {
      int const comparison = compare(key, node->getKey());
      if (comparison > 0) {
        if (!node->getGreaterChild()) break;
        node = node->getGreaterChild();
      }
      else if (comparison < 0) {
        if (!node->getLesserChild()) break;
        node = node->getLesserChild();
      }
      else {
        break;
      }
    }
    return node;
  }

  NodeType* findNearestGTE(KeyType const key) const {
    NodeType* node = findNearest(key);
    while (node && compare(node->getKey(), key) < 0) {
      node = node->getGreaterNeighbor();
    }
    return node;
  }

  NodeType* findNearestLTE(KeyType const key) const {
    NodeType* node = findNearest(key);
    while (node && compare(node->getKey(), key) > 0) {
      node = node->getLesserNeighbor();
    }
    return node;
  }

  bool remove(KeyType const key) {
    if (NodeType* const node = find(key)) {
      size -= node->getValues().size();
      removeNode(node);
      return true;
    }

    return false;
  }

  bool remove(KeyType const key, ValueType const value) {
    if (NodeType* const node = find(key)) {
      std::vector<ValueType> values = node->getValues();
      for (auto iter = values.begin(); iter != values.end(); ++iter) {
        ValueType const candidate = (*iter);
        if (candidate == value) {
          values.erase(iter);
          size -= 1;
          if (values.size() == 0) {
            removeNode(node);
          }
          return true;
        }
      }
    }

    return false;
  }

  inline void preorder(std::function<void (NodeType*)> const fn) const {
    preorder(fn, root);
  }

  void preorder(std::function<void (NodeType*)> const fn, NodeType* const node) const {
    if (node) {
      fn(node);
      preorder(fn, node->getLesserChild());
      preorder(fn, node->getGreaterChild());
    }
  }

  inline void inorder(std::function<void (NodeType*)> const fn) const {
    inorder(fn, root);
  }

  void inorder(std::function<void (NodeType*)> const fn, NodeType* const node) const {
    if (node) {
      inorder(fn, node->getLesserChild());
      fn(node);
      inorder(fn, node->getGreaterChild());
    }
  }

  inline void postorder(std::function<void (NodeType*)> const fn) const {
    postorder(fn, root);
  }

  void postorder(std::function<void (NodeType*)> const fn, NodeType* const node) const {
    if (node) {
      postorder(fn, node->getLesserChild());
      postorder(fn, node->getGreaterChild());
      fn(node);
    }
  }

  auto getRange(KeyType const lower_key, KeyType const upper_key) const {
    auto iter = new RangeIterator<NodeType, KeyType>();
    if (NodeType* const node = findNearestGTE(lower_key)) {
      iter->setNode(node)->setCompare(compare)->setUpperKey(upper_key);
    }
    return iter;
  }

  auto getNeighbors(
      KeyType const key,
      unsigned int const n_less,
      unsigned int const n_greater) const {

    auto iter = new RangeIterator<NodeType, KeyType>();

    if (NodeType* const node = findNearest(key)) {
      int const comparison = compare(node->getKey(), key);

      NodeType* lesser_neighbor = (comparison < 0)
        ? node
        : node->getLesserNeighbor();

      NodeType* greater_neighbor = (comparison > 0)
        ? node
        : node->getGreaterNeighbor();

      if (lesser_neighbor) {
        for (unsigned int n = 1; n < n_less; ++n) {
          if (!lesser_neighbor->getLesserNeighbor()) break;
          lesser_neighbor = lesser_neighbor->getLesserNeighbor();
        }
      }

      if (greater_neighbor) {
        for (unsigned int n = 1; n < n_greater; ++n) {
          if (!greater_neighbor->getGreaterNeighbor()) break;
          greater_neighbor = greater_neighbor->getGreaterNeighbor();
        }
      }

      NodeType* least_neighbor = (lesser_neighbor != nullptr)
        ? lesser_neighbor
        : node;

      KeyType const upper_key = (greater_neighbor != nullptr)
        ? greater_neighbor->getKey()
        : node->getKey();

      iter->setNode(least_neighbor)->setCompare(compare)->setUpperKey(upper_key);
    }

    return iter;
  }

  auto getNearestNeighbors(
      KeyType const key,
      unsigned int const k_neighbors,
      std::function<double (KeyType, KeyType)> const distance) const {

    auto iter = new NearestNeighborIterator<NodeType, KeyType>();

    if (NodeType* const node = findNearest(key)) {
      iter->setKey(key)->setDistance(distance)->setLimit(k_neighbors)->setNode(node);
    }

    return iter;
  }

  virtual void addDescendant(NodeType* ancestor, NodeType* descendant) = 0;
  virtual void removeNode(NodeType* node) = 0;

protected:
  unsigned int size = 0;
  std::function<int (KeyType, KeyType)> compare;
  NodeType* root = nullptr;

private:

  NodeType* buildNode(KeyType const key, ValueType const value) {
    NodeType* node = new NodeType();
    node->setKey(key)->addValue(value);
    return node;
  }
};

}

#endif
