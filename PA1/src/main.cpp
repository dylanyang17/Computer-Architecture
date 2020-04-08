#include <trace.hpp>
#include <cache.hpp>
#include <replacement_strategy.hpp>
#include <brute_force_cache.hpp>
#include <cstdio>
#include <cstdlib>

bool handleRead(int argc, char *argv[], char *&filename, int &blockSize, int &ways, int &strategy, int &isWriteAllocate, int &isWriteBack) {
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
    isWriteBack = atoi(argv[6]);

    if(!(blockSize == 8 || blockSize == 32 || blockSize == 64))
      fail = true;
    if(!(ways == 1 || ways == 2 || ways == 4 || ways == 8 || ways == -1))
      fail = true;
    if(!(strategy == 0 || strategy == 1 || strategy == 2))
      fail = true;
    if(!(isWriteAllocate == 0 || isWriteAllocate == 1))
      fail = true;
    if(!(isWriteBack == 0 || isWriteBack == 1))
      fail = true;
  }
  if (fail) {
    printf("\nUsage: bash run.sh <filename> <blockSize> <ways> <strategy> <isWriteAllocate> <isWriteBack>\n"
            "Parameters:\n"
            "  <filename>: the path of trace file, such as \"trace/gcc.trace\"\n"
            "  <blockSize>: 8, 32 or 64 (Byte)\n"
            "  <ways>: 1, 2, 4, 8 or -1 (means full)\n"
            "  <strategy>: 0 (LRU), 1 (RANDOM), 2 (BINARY_TREE)\n"
            "  <isWriteAllocate>: 0 or 1\n"
            "  <isWriteBack>: 0 or 1\n");
    return false;
  } else {
    printf("\nRunning:\n"
            "  <filename>: %s\n"
            "  <blockSize>: %d\n"
            "  <ways>: %d\n"
            "  <strategy>: %s\n"
            "  <isWriteAllocate>: %s\n"
            "  <isWriteBack>: %s\n",
            filename, blockSize, ways, (strategy == 0 ? "LRU" : ((strategy == 1) ? "RANDOM" : "BINARY_TREE")),
            (isWriteAllocate ? "true" : "false"), (isWriteBack ? "true" : "false"));
    return true;
  }
}

int main(int argc, char *argv[]) {
  // 输入并且保证参数正确性
  bool fail = false;
  int blockSize, ways, strategy, isWriteAllocate, isWriteBack;
  char *filename;
  if (!handleRead(argc, argv, filename, blockSize, ways, strategy, isWriteAllocate, isWriteBack)) {
    return 0;
  }

  Trace *trace = new Trace();
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error: File \"%s\" does not exist.\n", filename);
  } else {
    FILE *logFile = fopen("logs/", "w");
    trace->readItems(file);
    Cache *cache = new Cache(blockSize, ways, static_cast<StrategyType>(strategy),
      isWriteAllocate, isWriteBack);
    
    int cnt = 0;
    printf("tot: %d\n", trace->size());
    for (int i = 0; i < trace->size(); ++i) {
      bool suc = cache->visit((*trace)[i].addr, (*trace)[i].type);
      cnt += suc;
      // if (i % 1000 == 0) printf("%d\n", i);
    }
    printf("Rate: %.6f\n", 1 - (double)cnt / trace->size());
  }
  delete trace;
  fclose(file);
  return 0;
}