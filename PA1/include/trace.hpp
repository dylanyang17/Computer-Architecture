#ifndef TRACE_HPP
#define TRACE_HPP

#include <vector>
#include <cassert>
#include <cstdio>
#include <cctype>

enum class TraceType: char {DEFAULT='0', READ='r', WRITE='w', LOAD='l', STORE='s'};

struct TraceItem {
  // 用于存放Trace条目
  unsigned long long addr;
  TraceType type;

  TraceItem(const TraceType &type, const unsigned long long &addr) {
    this->type = type;
    this->addr = addr;
  }
};

class Trace {
 public:
  static char readChar(FILE*);
  
  void readItems(FILE*);
  
  int size() {
    return static_cast<int>(items.size());
  }
  
  TraceItem& operator[] (int ind) {
    assert(ind < static_cast<int>(items.size()));
    return items[ind];
  }

 private:
  std::vector<TraceItem> items;
};

#endif