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
} // namespace utils


#endif