#ifndef __VST_NEAREST_NEIGHBOR_ITERATOR_H__
#define __VST_NEAREST_NEIGHBOR_ITERATOR_H__

#include <functional>
#include <math.h>

#include "iterator.h"

namespace vst {

template <class NodeType, class KeyType>
class NearestNeighborIterator : public Iterator<NodeType*> {
public:
  using Iterator<NodeType*>::Iterator;

  inline auto setKey(KeyType const key) {
    this->key = key;
    return this;
  }

  inline auto setDistance(std::function<double (KeyType, KeyType)> const distance) {
    this->distance = distance;
    return this;
  }

  inline auto setLimit(unsigned int const limit) {
    this->limit = limit;
    return this;
  }

  /**
   * This should be the last setter called ...
   */
  NearestNeighborIterator<NodeType,KeyType>* setNode(NodeType* const node) {
    if (limit > 0) {
      this->has_advanced = false;
      this->next_element = findNearest(node);
      setLesserNeighbor(this->next_element->getLesserNeighbor());
      setGreaterNeighbor(this->next_element->getGreaterNeighbor());
      index = 1;
    }
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && index < limit) {
      if (d_lesser_neighbor < d_greater_neighbor) {
        this->next_element = lesser_neighbor;
        setLesserNeighbor(lesser_neighbor->getLesserNeighbor());
        this->has_advanced = false;
        index += 1;
      }
      else if (greater_neighbor) {
        this->next_element = greater_neighbor;
        setGreaterNeighbor(greater_neighbor->getGreaterNeighbor());
        this->has_advanced = false;
        index += 1;
      }
    }
  }

private:
  std::function<double (KeyType, KeyType)> distance;
  NodeType* lesser_neighbor;
  NodeType* greater_neighbor;
  double d_lesser_neighbor;
  double d_greater_neighbor;
  KeyType key;
  unsigned int limit;
  unsigned int index;

  void setLesserNeighbor(NodeType* const lesser_neighbor) {
    this->lesser_neighbor = lesser_neighbor;
    this->d_lesser_neighbor = (lesser_neighbor != nullptr)
      ? distance(lesser_neighbor->getKey(), key)
      : INFINITY;
  }

  void setGreaterNeighbor(NodeType* const greater_neighbor) {
    this->greater_neighbor = greater_neighbor;
    this->d_greater_neighbor = (greater_neighbor != nullptr)
      ? distance(greater_neighbor->getKey(), key)
      : INFINITY;
  }

  NodeType* findNearest(NodeType* node) {
    NodeType* lesser_neighbor = node->getLesserNeighbor();
    NodeType* greater_neighbor = node->getGreaterNeighbor();
    while (true) {
      if (lesser_neighbor) {
        if (greater_neighbor) {
          double const d_node = distance(node->getKey(), key);
          double const d_lesser_neighbor = distance(lesser_neighbor->getKey(), key);
          double const d_greater_neighbor = distance(greater_neighbor->getKey(), key);
          if (d_lesser_neighbor < d_node) {
            if (d_greater_neighbor < d_lesser_neighbor) {
              node = greater_neighbor;
              greater_neighbor = node->getGreaterNeighbor();
            }
            else {
              node = lesser_neighbor;
              lesser_neighbor = node->getLesserNeighbor();
            }
          }
          else if (d_greater_neighbor < d_node) {
            node = greater_neighbor;
            greater_neighbor = node->getGreaterNeighbor();
          }
          else {
            return node;
          }
        }
        else if (distance(lesser_neighbor->getKey(), key) < distance(node->getKey(), key)) {
          node = lesser_neighbor;
          lesser_neighbor = node->getLesserNeighbor();
        }
        else {
          return node;
        }
      }
      else if (greater_neighbor) {
        if (distance(greater_neighbor->getKey(), key) < distance(node->getKey(), key)) {
          node = greater_neighbor;
          greater_neighbor = node->getGreaterNeighbor();
        }
      }
      else {
        return node;
      }
    }
  }
};

}

#endif
