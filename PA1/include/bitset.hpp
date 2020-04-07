#ifndef BITSET_HPP
#define BITSET_HPP

#include <cassert>

template<int bnum>
class Bitset {
  // bnum 为位数
 public:
  static const int ITEM_BITS = 8;  // 一个 char 的位数

  bitset() {
    int cnum = (bnum + 7) / 8;
    data = new char[cnum];
  }

  ~bitset() {
    delete[] data;
  }

  int get(int ind) {
    assert(ind >= 0 && ind < bnum);
    return ((data[ind >> 3] >> (ind & 0x7)) & 1);
  }

  void set(int ind) {
    assert(ind >= 0 && ind < bnum);
    data[ind >> 3] |= (1 << (ind & 0x7));
  }

  void unset(int ind) {
    assert(ind >= 0 && ind < bnum);
    data[ind >> 3] &= (~(1 << (ind & 0x7)));
  }

  void flip(int ind) {
    assert(ind >= 0 && ind < bnum);
    data[ind >> 3] ^= (1 << (ind & 0x7));
  }

  void assignLow(int len, int val) {
    // 设置低 len 位，即第 [0, len) 位，变为值 val
    assert(len > 0 && len <= bnum && val < (1<<len));
    int ind = 0;
    while (len > 0) {
      if (len >= 8) {
        data[ind] = val & ((1 << 8) - 1);
        ind++;
        val >>= 8;
        len -= 8;
      } else {
        data[ind] &= (~((1 << len) - 1));
        data[ind] |= val;
        len = 0;
      }
    }
  }

 private:
  char *data;
};

#endif