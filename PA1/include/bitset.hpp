#ifndef BITSET_HPP
#define BITSET_HPP

#include <cassert>
#include <cstring>

class Bitset {
 public:
  Bitset() {
    bnum = 0;
    data = nullptr;
  }

  Bitset(int bnum) {
    resize(bnum);
  }

  ~Bitset() {
    if (data != nullptr)
      delete[] data;
  }

  void resize(int bnum) {
    if (data != nullptr) {
      delete[] data;
    }
    this->data = new char[(bnum + 7) / 8];
    this->bnum = bnum;
    memset(data, 0, sizeof(char) * ((bnum + 7) / 8));
  }

  int get(int ind) {
    assert(ind >= 0 && ind < bnum);
    return ((data[ind >> 3] >> (ind & 0x7)) & 1);
  }

  void set(int ind) {
    assert(ind >= 0 && ind < bnum);
    data[ind >> 3] |= (1 << (ind & 0x7));
  }

  void set(int ind, bool num) {
    if (num)
      set(ind);
    else
      unset(ind);
  }

  void unset(int ind) {
    assert(ind >= 0 && ind < bnum);
    data[ind >> 3] &= (~(1 << (ind & 0x7)));
  }

  void flip(int ind) {
    assert(ind >= 0 && ind < bnum);
    data[ind >> 3] ^= (1 << (ind & 0x7));
  }

  void assignSeg(int beg, int end, unsigned long long val) {
    // 设置第 [beg, end) 位，变为值 val
    assert(beg >= 0 && beg < bnum);
    assert(end > 0 && end <= bnum);
    int tnum = end - beg;
    assert(val >= 0 && val < (1ull << tnum));
    for (int i = beg; i < end; ++i) {
      if (val & 1) {
        set(i);
      } else {
        unset(i);
      }
      val >>= 1;
    }
  }

  unsigned long long getSeg(int beg, int end) {
    // 获取第 [beg, end) 位
    assert(beg >= 0 && beg < bnum);
    assert(end > 0 && end <= bnum);
    unsigned long long ret = 0;
    for (int i = end-1; i >= beg; --i) {
      ret <<= 1;
      ret |= get(i);
    }
    return ret;
  }

 private:
  char *data;
  int bnum;
};

#endif