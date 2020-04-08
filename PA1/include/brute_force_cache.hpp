#include <replacement_strategy.hpp>
#include <trace.hpp>
#include <vector>

struct BruteForceCacheItem {
  bool valid, dirty;
  unsigned long long tag;

  BruteForceCacheItem() {
    valid = dirty = false;
    tag = 0;
  }
};

class BruteForceCacheGroup {
 public:
  void init(int ways, int tagBits, StrategyType strategyType, bool isWriteAllocate, bool isWriteBack) {
    this->ways = ways;
    this->tagBits = tagBits;
    this->isWriteAllocate = isWriteAllocate;
    this->isWriteBack = isWriteBack;
    this->strategyType = strategyType;
    items = new BruteForceCacheItem[ways];
    LRUVec.clear();
    for (int i = 0; i < ways; ++i) LRUVec.push_back(i);
  }

  int findEmpty() {
    // 寻找空位置，无则返回 -1
    int ret = -1;
    for (int i = 0; i < ways; ++i) {
      if (items[i].valid == false) {
        ret = i;
        break;
      }
    }
    return ret;
  }

  int find(unsigned long long tag) {
    // 寻找有效且同 tag 匹配的项
    int ret = -1;
    for (int i = 0; i < ways; ++i) {
      if (items[i].valid && items[i].tag == tag) {
        ret = i;
        break;
      }
    }
    return ret;
  }

  void LRUMakeTop(int val) {
    // 把某个值（即cache组中下标）置顶
    int pos = -1;
    for (int i = 0; i < LRUVec.size(); ++i) {
      if (LRUVec[i] == val) {
        pos = i;
        break;
      }
    }
    assert(pos != -1);
    int tmp = LRUVec[pos];
    LRUVec.erase(LRUVec.begin() + pos);
    LRUVec.insert(LRUVec.begin(), tmp);
  }

  bool BTVisit(unsigned long long tag, TraceType type) {
    // 使用 BT 方式 visit，假设写分配，且暂不处理 dirty
    assert(0);
  }

  bool LRUVisit(unsigned long long tag, TraceType type) {
    // 使用 LRU 方式 visit，假设写分配，且暂不处理 dirty
    int i = find(tag), ei = findEmpty();
    bool ret = false;
    if (i == -1) {
      // Miss
      if (ei == -1) {
        // 已满，利用堆栈
        ei = LRUVec.back();
        LRUMakeTop(LRUVec.back());
      } else {
        // 未满
        LRUMakeTop(ei);
      }
      items[ei].dirty = false;
      items[ei].valid = true;
      items[ei].tag = tag;
      ret = false;
    } else {
      // Hit
      LRUMakeTop(i);
      ret = true;
    }
    return ret;
  }

  bool visit(unsigned long long tag, TraceType type) {
    assert(isWriteAllocate);
    assert(isWriteBack);
    // NOTE: 暂不处理 dirty
    if (strategyType == StrategyType::LRU) {
      return LRUVisit(tag, type);
    } else if (strategyType == StrategyType::BINARY_TREE) {
      return BTVisit(tag, type);
    } else if (strategyType == StrategyType::RANDOM) {
      // TODO
    }
    else assert(0);
  }

 private:
  bool isWriteAllocate, isWriteBack;
  int ways, tagBits;
  StrategyType strategyType;
  BruteForceCacheItem *items;
  std::vector<int> LRUVec;
};

class BruteForceCache {
 public:
  static const int TOTAL_SIZE = 128 * 1024;
  static const int ADDR_BITS = 64;

  BruteForceCache(int blockSize, int ways, StrategyType strategyType, bool isWriteAllocate, bool isWriteBack) {
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
    this->groups = new BruteForceCacheGroup[groupNum];
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
  BruteForceCacheGroup *groups;
};