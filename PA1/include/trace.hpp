#ifndef TRACE_HPP
#define TRACE_HPP

#include <vector>
#include <cassert>

enum class TraceType {READ, WRITE, LOAD, STORE};

struct TraceItem {
  // 用于存放Trace条目
  unsigned long long addr;
  TraceType type;
};

class Trace {
 public:
  void readAll();
  TraceItem& operator[] (int ind) {
    assert(ind < items.size());
    return items[ind];
  }

 private:
  std::vector<TraceItem> items;
};

#endif