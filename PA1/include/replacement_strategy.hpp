#ifndef REPLACEMENT_STRATEGY
#define REPLACEMENT_STRATEGY

#include <bitset.hpp>
#include <utils.hpp>

enum class StrategyType: int {
  LRU = 0, RAMDOM = 1, BINARY_TREE = 2
};

class ReplacementStrategy {
 public:
  virtual int placeIn(int i) = 0;
};

class LRUStrategy: public ReplacementStrategy {
 public:
  LRUStrategy() = delete;

  LRUStrategy(int ways) {
    this->len = 0;      // 当前堆栈大小
    this->ways = ways;  // 相联数，即最大存储数
    this->itemSize = utils::lg2(ways);
    data.resize(itemSize * ways);
  }

  int get(int ind) {
    // 获取堆栈中第 ind 项
    data.getSeg(ind * itemSize, (ind+1) * itemSize);
  }

  void set(int ind, int val) {
    // 将堆栈中第 ind 项设置为 val
    data.assignSeg(ind * itemSize, (ind+1) * itemSize, val);
  }

  int makeTop(int ind) {
    // 将堆栈中第 ind 项置顶，并且返回它的值
    int tmp = get(ind);
    for (int i = ind - 1; i >= 0; --i) {
      set(i + 1, get(i));
    }
    set(0, ind);
    return tmp;
  }

  int find(int val) {
    // 在堆栈中寻找是否出现 val，未出现则返回 -1
    for (int i = 0; i < len; ++i) {
      if (get(i) == val)
        return i;
    }
    return -1;
  }

  int placeIn(int val) override {
    // 访问了组中下标为 val 的项，-1 表示访问的项不在组中（即组满且发生替换的情况）
    // 三种情况——未满、满并且未发生替换、满并且发生替换
    // 发生替换时返回换出者下标，否则返回 -1
    int ret = -1;
    if (val == -1) {
      // 满且发生替换
      assert(len == ways);
      ret = makeTop(len - 1);
    } else if (len < ways) {
      // 未满
      set(len++, val);
      ret = -1;
    } else {
      // 满且未发生替换，将堆栈中第 i 项置顶
      int i = find(val);
      assert(i != -1);
      makeTop(i);
      ret = -1;
    }
    return ret;
  }

 private:
  Bitset data;
  int len, ways, itemSize;
};

#endif