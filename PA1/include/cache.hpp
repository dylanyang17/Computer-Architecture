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
  void init(int ways, int tagBits, StrategyType strategyType, bool isWriteAllocate, bool isWriteBack) {
    this->ways = ways;
    this->tagBits = tagBits;
    this->isWriteAllocate = isWriteAllocate;
    this->isWriteBack = isWriteBack;
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

  int find(unsigned long long tag) {
    // 寻找组内是否有相应 tag，找到则返回相应下标，否则返回 -1
    for (int i = 0; i < ways; ++i) {
      if (getValid(i) && getTag(i) == tag)
        return i;
    }
    return -1;
  }

  int findEmpty() {
    // 寻找组内是否有 invalid 的位置，找到则返回相应下标，否则返回 -1
    for (int i = 0; i < ways; ++i) {
      if (!getValid(i))
        return i;
    }
    return -1;
  }

  bool visit(unsigned long long tag, TraceType type) {
    // 在这个组中访问 tag
    bool ret = false;
    int i = find(tag), ei = -1;
    if (i != -1) ret = true;
    else ei = findEmpty();
    
    if (i == -1) {
      // Miss
      if ((type == TraceType::DEFAULT || type == TraceType::LOAD || type == TraceType::READ) ||
          ((type == TraceType::STORE || type == TraceType::WRITE) && isWriteAllocate)) {
        // 需要进行分配
        if (ei != -1) {
          // 有空位
          strategy->placeIn(ei);
        } else {
          // 没有空位，利用替换策略找到替换出去的下标
          ei = strategy->placeIn(-1);
        }
        setValid(ei, true);
        setDirty(ei, false);
        setTag(ei, tag);
      }
      ret = false;
    } else {
      // Hit
      strategy->placeIn(i);
      if ((type == TraceType::STORE || type == TraceType::WRITE) && isWriteBack) {
        setDirty(i, true);
      }
      ret = true;
    }
    return ret;
  }

 private:
  bool isWriteAllocate, isWriteBack;
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
      this->groups[i].init(ways, tagBits, strategyType, isWriteAllocate, isWriteBack);
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