#ifndef REPLACEMENT_STRATEGY
#define REPLACEMENT_STRATEGY

#include <bitset.hpp>
#include <utils.hpp>
#include <random>

enum class StrategyType: int {
  LRU = 0, RANDOM = 1, BINARY_TREE = 2
};

class ReplacementStrategy {
 public:
  virtual int placeIn(int val) = 0;
};

class RandomStrategy: public ReplacementStrategy {
 public:
  RandomStrategy() = delete;

  RandomStrategy(int ways) {
    srand(19961202);  // 固定种子，便于重复实验
    this->ways = ways;
  }

  int placeIn(int val) {
    return rand() % ways;
  }

 private:
  int ways;
};

class BinaryTreeStrategy: public ReplacementStrategy {
 public:
  BinaryTreeStrategy() = delete;

  BinaryTreeStrategy(int ways) {
    this->ways = ways;
    this->h = utils::lg2(ways);
    data.resize(ways);
  }

  bool getAllValid() {
    return data.get(ways - 1);
  }

  void setAllValid(bool valid) {
    data.set(ways - 1, valid);
  }

  void visitAlongTree(int val) {
    // 沿着树访问 val
    int now = 0;
    for (int i = h - 1; i >= 0; --i) {
      bool lr = ((val >> i) & 1);
      data.set(now, lr ^ 1);
      now = ((now << 1) + 1 + lr);
    }
  }

  int visitAlongTree() {
    // 沿着树找到替换结点
    int now = 0;
    for (int i = h - 1; i >= 0; --i) {
      bool lr = data.get(now);
      data.set(now, lr ^ 1);
      now = ((now << 1) + 1 + lr);
    }
    return now - (ways - 1);
  }

  int placeIn(int val) override {
    // val 为 -1 时表示发生满替换
    if (val == -1) {
      return visitAlongTree();
    } else {
      visitAlongTree(val);
      return -1;
    }
  }

 private:
  Bitset data;
  int ways;
  int h;  // 树高（注意单个根结点时树高为 0，但此处树高算上了未存储的叶子结点）
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
    set(0, tmp);
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
    // 三种大情况——未满且缺失、满且缺失、满/未满且命中
    // 发生替换时返回换出者下标，否则返回 -1
    int ret = -1;
    if (val == -1) {
      // 满且缺失
      assert(len == ways);
      ret = makeTop(len - 1);
    } else {
      int i = find(val);
      if (i != -1) {
        // 满/未满且命中
        makeTop(i);
        ret = -1;
      } else if (len < ways) {
        // 未满且缺失
        set(len++, val);
        makeTop(len - 1);
        ret = -1;
      }
    }
    return ret;
  }

 private:
  Bitset data;
  int len, ways, itemSize;
};

#endif