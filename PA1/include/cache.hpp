#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <trace.hpp>
#include <bitset.hpp>
#include <replacement_strategy.hpp>

class CacheGroup {
  // 一个组，大小为组相联数，即有 ways 项
  // 对于每一项，使用 Bitset 存储，共 tagBits + 2 位
  // 每项存储形式为 valid + dirty + tag，此处加号表示连接，左边为高位
 public:
  void init(int ways, int tagBits, StrategyType strategyType) {
    this->ways = ways;
    this->tagBits = tagBits;
    items = new Bitset[ways];
    for (int i = 0; i < ways; ++i) {
      items[i].resize(tagBits + 2);
    }
    switch (strategyType)
    {
    case StrategyType::LRU:
      strategy = new LRUStrategy(ways);
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

  unsigned long long getTag(int i) {
    // 获得第 i 项的 tag
    return items[i].getSeg(0, tagBits);
  }

  void setTag(int i, unsigned long long tag) {
    // 设置第 i 项的 tag
    items[i].assignSeg(0, tagBits, tag);
  }

  bool getDirty(int i) {
    // 获取第 i 项是否 dirty
    return items[i].get(tagBits);
  }

  void setDirty(int i, bool dirty) {
    // 设置第 i 项是否 dirty
    items[i].set(tagBits, dirty);
  }

  bool getValid(int i) {
    // 获取第 i 项是否 valid
    return items[i].get(tagBits + 1);
  }

  void setValid(int i, bool valid) {
    // 设置第 i 项是否 valid
    items[i].set(tagBits + 1, valid);
  }

  bool visit(unsigned long long tag, TraceType type) {
    // 在这个组中访问 tag
    switch (type)
    {
    case TraceType::DEFAULT:
    case TraceType::READ:
    case TraceType::LOAD:
      
      break;
    case TraceType::WRITE:
    case TraceType::STORE:
      // TODO
      break;
    default:
      assert(0);
      break;
    }
  }

 private:
  int ways, tagBits;
  ReplacementStrategy *strategy;
  Bitset *items;
};

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
    this->indexBits = utils::lg2(groupNum);
    this->tagBits = ADDR_BITS - blockBits - indexBits;
    this->groups = new CacheGroup[groupNum];
    for (int i = 0; i < groupNum; ++i)
      this->groups[i].init(ways, tagBits, strategyType);
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
    return groups[index].visit(tag, type);
  }

 private:
  int blockSize;  // 8, 32 or 64
  int blockNum;   // number of blocks
  int ways;       // 1, 4, 8 or <number of blocks>
  int groupNum;   // number of groups, which is euqal to blockNum/ways
  int blockBits, indexBits, tagBits;  // bits of block, index and tag, which's sum is 64
  bool isWriteAllocate, isWriteBack;
  CacheGroup *groups;
};

#endif