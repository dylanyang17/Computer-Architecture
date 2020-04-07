#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <trace.hpp>
#include <bitset.hpp>
#include <replacement_strategy.hpp>

class Cache {
  // 用于模拟硬件 Cache，总大小为 128KB
 public:
  static const int TOTAL_SIZE = 128 * 1024;
  static const int ADDR_BITS = 64;
    
  Cache() = delete;

  // 注意传入的 ways 为 -1 表示全相联，需要计算实际 ways
  Cache(int blockSize, int ways, StrategyType strategyType, bool isWriteAllocate, bool isWriteBack) {
    this->isWriteAllocate = isWriteAllocate;
    this->isWriteBack = isWriteBack;
    this->blockSize = blockSize;
    this->blockNum = TOTAL_SIZE / blockSize;
    if (ways == -1) {
      ways = blockNum;
    }
    this->groupNum = blockNum / ways;
    this->ways = ways;
    this->blockBits = utils::lg2(blockSize);
    this->indexBits = utils::lg2(TOTAL_SIZE / blockSize / ways);
    this->tagBits = ADDR_BITS - blockBits - indexBits;
    this->items = new Bitset[blockNum];
    for (int i = 0; i < blockNum; ++i) {
      this->items[i].resize(tagBits + 2);
    }

    // 设置每个组的 strategy
    this->strategy = new ReplacementStrategy*[groupNum];
    for (int i = 0; i < groupNum; ++i) {
      switch (strategyType)
      {
      case StrategyType::LRU:
        strategy[i] = new LRUStrategy(ways);
        break;
      case StrategyType::RAMDOM:
        // TODO
        break;
      case StrategyType::BINARY_TREE:
        // TODO
        break;
      default:
        assert(0);
        break;
      }
    }
  }

  void analyze(unsigned long long addr, unsigned long long &tag, int &index) {
    // 解析给定地址的tag以及index
    index = utils::getBitsSeg(addr, blockBits, blockBits + indexBits);
    tag = utils::getBitsSeg(addr, blockBits + indexBits, ADDR_BITS);
  }

  bool visit(unsigned long long addr, TraceType type) {
    unsigned long long tag;
    int index;
    analyze(addr, tag, index);
  }

 private:
  int blockSize;  // 8, 32 or 64
  int blockNum;   // number of blocks
  int ways;       // 1, 4, 8 or <number of blocks>
  int groupNum;   // number of groups, which is euqal to blockNum/ways
  int blockBits, indexBits, tagBits;  // bits of block, index and tag, which's sum is 64
  Bitset *items;
  ReplacementStrategy **strategy;
  bool isWriteAllocate, isWriteBack;
};

#endif