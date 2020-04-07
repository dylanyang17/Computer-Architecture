#ifndef UTILS_HPP
#define UTILS_HPP

namespace utils
{
  int lg2(int num) {
    int ret = 0;
    while (num != 1) {
      num >>= 1;
      ret++;
    }
    return ret;
  }

  unsigned long long getBitsSeg(unsigned long long num, int beg, int end) {
    // 获得数 num 的第 [beg, end) 低二进制位的值 (下标从 0 开始)
    assert(beg >= 0 && beg < 64);
    assert(end > 0 && end <= 64);
    return ((num >> beg) & ((1ull << (end-beg)) - 1));
  }
} // namespace utils


#endif