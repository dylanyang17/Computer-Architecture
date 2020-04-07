#ifndef REPLACEMENT_STRATEGY
#define REPLACEMENT_STRATEGY

#include <bitset.hpp>
#include <utils.hpp>

enum class StrategyType: int {
  LRU = 0, RAMDOM = 1, BINARY_TREE = 2
};

class ReplacementStrategy {
 public:
  virtual void replace() = 0;
};

class LRUStrategy: public ReplacementStrategy {
 public:
  LRUStrategy() = delete;

  LRUStrategy(int ways) {
    this->ways = ways;
    this->itemSize = utils::lg2(ways);
    data.resize(itemSize * ways);
  }

  void replace() override {
    
  }

 private:
  Bitset data;
  int ways;
  int itemSize;
};

#endif