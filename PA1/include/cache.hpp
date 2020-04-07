#ifndef CACHE_HPP
#define CACHE_HPP

#include <bitset.hpp>

class Cache {
  // 用于模拟硬件 Cache，总大小为 128KB
  // 注意到块大小可能为 8B, 32B, 64B，
  // 故元数据(Tag加上Valid和Dirty位)最多占用 56 位，最少占用 53 位，
  // 故每项将使用 7 个 char
 public:
  static const int TOTAL_SIZE = 128 * 1024;
  static const int ITEM_SIZE = 7;
    
  Cache() = delete;

  // 注意传入的 ways 为 -1 表示全相联，需要计算实际 ways
  Cache(int blockSize, int ways) {
    this->blockSize = blockSize;
    this->blockNum = TOTAL_SIZE / blockSize;
    if (ways == -1) {
      ways = blockNum;
    }
    this->groupNum = this->blockNum / ways;
    this->ways = ways;
    this->items.resize(this->blockNum * ITEM_SIZE * 8);
  }

 private:
  int blockSize;  // 8, 32 or 64
  int blockNum;   // number of blocks
  int ways;       // 1, 4, 8 or <number of blocks>
  int groupNum;   // number of groups, which is euqal to blockNum/ways
  Bitset items;
};

#endif