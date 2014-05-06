#include <vector>

#include <gtest/gtest.h>

#include "../../vst/iterator.h"

using namespace vst;

template <class ElementType>
class SingletonIterator : public Iterator<ElementType> {
public:
  using Iterator<ElementType>::Iterator;
  inline SingletonIterator* setElement(ElementType const element) {
    this->element = element;
    return this;
  }
protected:
  void advance() {
    if (this->has_advanced && !has_advanced_once) {
      this->has_advanced_once = true;
      this->has_advanced = false;
      this->next_element = element;
    }
  }
private:
  ElementType element;
  bool has_advanced_once = false;
};

template <class ElementType>
class PairIterator : public Iterator<ElementType> {
public:
  using Iterator<ElementType>::Iterator;
  inline PairIterator* setPair(ElementType const a, ElementType const b) {
    this->a = a;
    this->b = b;
    return this; }
protected:
  void advance() {
    if (this->has_advanced && index < 2) {
      this->has_advanced = false;
      this->next_element = (index < 1) ? a : b;
      this->index += 1;
    }
  }
private:
  int index = 0;
  ElementType a, b;
};

TEST(IteratorTest, TestMap) {
  Iterator<double>* iter = (new SingletonIterator<int>())->setElement(2)->map<double>([](int const i) {
    return i * i;
  });

  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(4.0, iter->peek());
  ASSERT_EQ(4.0, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(4.0, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}

TEST(IteratorTest, TestToVector) {
  PairIterator<int> iter;
  iter.setPair(2,3);
  std::vector<int> vector = iter.to_vector();
  ASSERT_EQ(2, vector.size());
  ASSERT_EQ(2, vector.at(0));
  ASSERT_EQ(3, vector.at(1));
}

TEST(IteratorTest, TestEach) {
  int const element = 42;
  int called = 0;
  int called_element = 0;
  SingletonIterator<int> iter;
  iter.setElement(element);
  iter.each([&](int const element) -> bool {
    called += 1;
    called_element = element;
    return true;
  });
  ASSERT_EQ(1, called);
  ASSERT_EQ(element, called_element);
}

TEST(IteratorTest, TestEachOnFalse) {
  int const a = 10;
  int const b = 15;
  int called = 0;
  PairIterator<int> iter;
  iter.setPair(a,b);
  iter.each([=, &called](int const i) {
    called += 1;
    return i != a;
  });
  ASSERT_EQ(1, called);
}

TEST(IteratorTest, TestSelect) {
  Iterator<int>* iter = (new PairIterator<int>())->setPair(2,3)->select([](int const i) {return i == 2;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->select([](int const i) {return i == 3;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}

TEST(IteratorTest, TestExclude) {
  Iterator<int>* iter = (new PairIterator<int>())->setPair(2,3)->exclude([](int const i) {return i == 2;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->exclude([](int const i) {return i == 3;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}

TEST(IteratorTest, TestTake) {
  Iterator<int>* iter = (new PairIterator<int>())->setPair(2,3)->take(0);
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());

  iter = (new PairIterator<int>())->setPair(2,3)->take(1);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->take(2);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->take(3);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}

TEST(IteratorTest, TestTakeWhile) {
  Iterator<int>* iter = (new PairIterator<int>())->setPair(2,3)->takeWhile([](int const i) {return i % 2 == 0;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->takeWhile([](int const i) {return i % 2 == 1;});
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}

TEST(IteratorTest, TestDrop) {
  Iterator<int>* iter = (new PairIterator<int>())->setPair(2,3)->drop(0);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->drop(1);
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->drop(2);
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->drop(3);
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}

TEST(IteratorTest, TestDropWhile) {
  Iterator<int>* iter = (new PairIterator<int>())->setPair(2,3)->dropWhile([](int const i) {return i % 2 == 0;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;

  iter = (new PairIterator<int>())->setPair(2,3)->dropWhile([](int const i) {return i % 2 == 1;});
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->peek());
  ASSERT_EQ(2, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(2, iter->next());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->peek());
  ASSERT_EQ(3, iter->peek());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_TRUE(iter->hasNext());
  ASSERT_EQ(3, iter->next());
  ASSERT_FALSE(iter->hasNext());
  ASSERT_FALSE(iter->hasNext());
  delete iter;
}
