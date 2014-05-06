#ifndef __VST_ITERATOR_H__
#define __VST_ITERATOR_H__

#include <functional>
#include <vector>

namespace vst {

template <class ElementType>
class Iterator {
public:

  Iterator() {
    // empty constructor
  }

  virtual ~Iterator() {
    // empty destructor
  }

  bool hasNext() {
    advance();
    return !has_advanced;
  }

  ElementType peek() {
    advance();
    return next_element;
  }

  ElementType next() {
    advance();
    ElementType next_element = this->next_element;
    this->next_element = {};
    has_advanced = true;
    return next_element;
  }

  std::vector<ElementType> to_vector() {
    std::vector<ElementType> vector;
    while (hasNext()) {
      vector.push_back(next());
    }
    return vector;
  }

  template <class MappedType>
  Iterator<MappedType>* map(std::function<MappedType (ElementType)> fn);

  bool each(std::function<bool (ElementType)> const fn) {
    while (hasNext()) {
      ElementType const element = next();
      if (!fn(element)) return false;
    }
    return true;
  }

  Iterator<ElementType>* select(std::function<bool (ElementType)> fn);
  Iterator<ElementType>* exclude(std::function<bool (ElementType)> fn);
  Iterator<ElementType>* take(unsigned int limit);
  Iterator<ElementType>* takeWhile(std::function<bool (ElementType)> fn);

  Iterator<ElementType>* drop(unsigned int const limit) {
    for (unsigned int i = 0; i < limit && hasNext(); ++i) {
      next();
    }
    return this;
  }

  Iterator<ElementType>* dropWhile(std::function<bool (ElementType)> const fn) {
    while (hasNext()) {
      ElementType element = peek();
      if (!fn(element)) break;
      next();
    }
    return this;
  }

protected:

  /** Specifies that the next element has been advanced, and that a new one
   * should be retrieved */
  bool has_advanced = true;

  /** Next element to return */
  ElementType next_element = {};

  /**
   * The expected behavior of this method is that it transitions to the next
   * element to return, if the current element has been returned (i.e. it is
   * null).
   */
  virtual void advance() = 0;
};

template <class ElementType, class MappedType>
class MapIterator : public Iterator<MappedType> {
public:
  using Iterator<MappedType>::Iterator;

  ~MapIterator() {
    delete iterator;
  }

  inline auto setFn(std::function<MappedType (ElementType)> const fn) {
    this->fn = fn;
    return this;
  }

  inline auto setIterator(Iterator<ElementType>* const iterator) {
    this->iterator = iterator;
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && iterator->hasNext()) {
      this->has_advanced = false;
      const ElementType element = iterator->next();
      this->next_element = fn(element);
    }
  }

private:
  std::function<MappedType (ElementType)> fn;
  Iterator<ElementType>* iterator;
};

template <class ElementType>
class SelectIterator : public Iterator<ElementType> {
public:
  using Iterator<ElementType>::Iterator;

  ~SelectIterator() {
    delete iterator;
  }

  inline auto setIterator(Iterator<ElementType>* const iterator) {
    this->iterator = iterator;
    return this;
  }

  inline auto setFn(std::function<bool (ElementType)> const fn) {
    this->fn = fn;
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && iterator->hasNext()) {
      ElementType element = iterator->next();
      while (!fn(element) && iterator->hasNext()) {
        element = iterator->next();
      }
      if (fn(element)) {
        this->has_advanced = false;
        this->next_element = element;
      }
    }
  }

private:
  Iterator<ElementType>* iterator;
  std::function<bool (ElementType)> fn;
};

template <class ElementType>
class ExcludeIterator : public Iterator<ElementType> {
public:
  using Iterator<ElementType>::Iterator;

  ~ExcludeIterator() {
    delete iterator;
  }

  inline auto setIterator(Iterator<ElementType>* const iterator) {
    this->iterator = iterator;
    return this;
  }

  inline auto setFn(std::function<bool (ElementType)> const fn) {
    this->fn = fn;
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && iterator->hasNext()) {
      ElementType element = iterator->next();
      while (fn(element) && iterator->hasNext()) {
        element = iterator->next();
      }
      if (!fn(element)) {
        this->has_advanced = false;
        this->next_element = element;
      }
    }
  }

private:
  Iterator<ElementType>* iterator;
  std::function<bool (ElementType)> fn;
};

template <class ElementType>
class TakeIterator : public Iterator<ElementType> {
public:
  using Iterator<ElementType>::Iterator;

  ~TakeIterator() {
    delete iterator;
  }

  inline auto setIterator(Iterator<ElementType>* const iterator) {
    this->iterator = iterator;
    return this;
  }

  inline auto setLimit(unsigned int const limit) {
    this->limit = limit;
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && index < limit && iterator->hasNext()) {
      this->next_element = iterator->next();
      this->has_advanced = false;
      this->index += 1;
    }
  }

private:
  Iterator<ElementType>* iterator;
  unsigned int index = 0;
  unsigned int limit;
};

template <class ElementType>
class TakeWhileIterator : public Iterator<ElementType> {
public:
  using Iterator<ElementType>::Iterator;

  ~TakeWhileIterator() {
    delete iterator;
  }

  inline auto setIterator(Iterator<ElementType>* const iterator) {
    this->iterator = iterator;
    return this;
  }

  inline auto setFn(std::function<bool (ElementType)> const fn) {
    this->fn = fn;
    return this;
  }

protected:

  void advance() {
    if (this->has_advanced && iterator->hasNext()) {
      ElementType element = iterator->peek();
      if (fn(element)) {
        this->next_element = iterator->next();
        this->has_advanced = false;
      }
    }
  }

private:
  Iterator<ElementType>* iterator;
  std::function<bool (ElementType)> fn;
};

template <class ElementType>
template <class MappedType>
Iterator<MappedType>* Iterator<ElementType>::map(
    std::function<MappedType (ElementType)> const fn) {
  auto iter = new MapIterator<ElementType, MappedType>();
  iter->setIterator(this)->setFn(fn);
  return iter;
}

template <class ElementType>
Iterator<ElementType>* Iterator<ElementType>::select(
    std::function<bool (ElementType)> const fn) {
  auto iter = new SelectIterator<ElementType>();
  iter->setIterator(this)->setFn(fn);
  return iter;
}

template <class ElementType>
Iterator<ElementType>* Iterator<ElementType>::exclude(
    std::function<bool (ElementType)> const fn) {
  auto iter = new ExcludeIterator<ElementType>();
  iter->setIterator(this)->setFn(fn);
  return iter;
}

template <class ElementType>
Iterator<ElementType>* Iterator<ElementType>::take(unsigned int const limit) {
  auto iter = new TakeIterator<ElementType>();
  iter->setIterator(this)->setLimit(limit);
  return iter;
}

template <class ElementType>
Iterator<ElementType>* Iterator<ElementType>::takeWhile(
    std::function<bool (ElementType)> const fn) {
  auto iter = new TakeWhileIterator<ElementType>();
  iter->setIterator(this)->setFn(fn);
  return iter;
}

}

#endif
