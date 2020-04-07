#include <trace.hpp>
#include <cache.hpp>
#include <cstdio>
#include <cstdlib>

bool handleRead(int argc, char *argv[], char *filename, int &blockSize, int &ways, int &strategy, int &isWriteAllocate, int &isWriteThrough) {
  // 处理输入并且保证参数正确性
  // 出现错误则返回 false
  bool fail = false;
  if (argc != 7) {
    fail = true;
  }
  if (!fail) {
    filename = argv[1];
    blockSize = atoi(argv[2]);
    ways = atoi(argv[3]);
    strategy = atoi(argv[4]);
    isWriteAllocate = atoi(argv[5]);
    isWriteThrough = atoi(argv[6]);

    if(!(blockSize == 8 || blockSize == 32 || blockSize == 64))
      fail = true;
    if(!(ways == 1 || ways == 4 || ways == 8 || ways == -1))
      fail = true;
    if(!(strategy == 0 || strategy == 1 || strategy == 2))
      fail = true;
    if(!(isWriteAllocate == 0 || isWriteAllocate == 1))
      fail = true;
    if(!(isWriteThrough == 0 || isWriteThrough == 1))
      fail = true;
  }
  if (fail) {
    printf("\nUsage: bash run.sh <filename> <blockSize> <ways> <strategy> <isWriteAllocate> <isWriteThrough>\n\
            \rParameters:\n\
            <filename>: the path of trace file, such as \"trace/gcc.trace\"\n\
            <blockSize>: 8, 32 or 64 (Byte)\n\
            <ways>: 1, 4, 8 or -1 (means full)\n\
            <strategy>: 0 (LRU), 1 (RANDOM), 2 (BINARY_TREE)\n\
            <isWriteAllocate>: 0 or 1\n\
            <isWriteThrough>: 0 or 1\n");
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  // 输入并且保证参数正确性
  bool fail = false;
  int blockSize, ways, strategy, isWriteAllocate, isWriteThrough;
  char *filename;
  if (!handleRead(argc, argv, filename, blockSize, ways, strategy, isWriteAllocate, isWriteThrough)) {
    return 0;
  }

  Trace *trace = new Trace();
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error: File \"%s\" does not exist.\n", filename);
  } else {
    trace->readItems(file);
    // Cache *cache = new Cache();
  }
  delete trace;
  return 0;
}