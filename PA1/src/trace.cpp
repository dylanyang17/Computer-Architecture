#include <trace.hpp>

char Trace::readChar(FILE *file) {
  // 读入一个非空格、回车的字符
  char c;
  while ((c = static_cast<char>(fgetc(file))) != EOF)
    if (!isspace(c)) break;
  return c;
}

void Trace::readItems(FILE* file) {
  // 从 file 处读入全部 Trace 条目
  items.clear();
  char c = readChar(file);
  bool flag = false;  // 为 true 表示每行只有地址
  if (c == '0') {
    // 读入到了 0x 前缀的 0
    flag = true;
  }
  rewind(file);
  while (true) {
    if (!flag) {
      if ((c = readChar(file)) == EOF) break; 
    }
    TraceType type = static_cast<TraceType>(c);
    unsigned long long addr;
    if (fscanf(file, "%llx", &addr) == EOF) break;
    items.push_back(TraceItem(type, addr));
  }
};