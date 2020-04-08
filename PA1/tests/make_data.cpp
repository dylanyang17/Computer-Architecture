#include <cstdio>
#include <cassert>
#include <random>
#include <ctime>

const int TOTAL_SIZE = 128 * 1024;
const int ADDR_BITS = 64;

int lg2(int num) {
  int ret = 0;
  while (num != 1) {
    num >>= 1;
    ret++;
  }
  return ret;
}

int ways, blockSize, len;

int blockNum, groupNum, blockBits, indexBits, tagBits;

unsigned long long getItem(unsigned long long tag, unsigned long long index, int offset = 0) {
  // offset 无用
  return (tag << (blockBits + indexBits)) | (index << blockBits) | offset;
}

unsigned long long rd(unsigned long long l, unsigned long long r) {
  return ((unsigned long long)rand() << 48) | ((unsigned long long)rand() << 32) | ((unsigned long long)rand() << 16) | ((unsigned long long)rand());
}

int main(int argc, char *argv[]) {
  // 暂时只使用一种格式——即仅地址的格式
  srand(time(NULL) + (long long)new int);
  assert(argc == 4);
  ways = atoi(argv[1]), blockSize = atoi(argv[2]), len = atoi(argv[3]);

  blockNum = TOTAL_SIZE / blockSize, groupNum = blockNum / ways;
  blockBits = lg2(blockSize);
  indexBits = lg2(groupNum);
  tagBits = ADDR_BITS - blockBits - indexBits;
  
  fprintf(stderr, "ways: %d, blockSize: %d, traceItems: %d\ntagBits: %d, indexBits: %d, blockBits: %d\n", 
    ways, blockSize, len, tagBits, indexBits, blockBits);

  for (int i = 0; i < len; ++i) {
    // 都在同一组内测试
    printf("0x%llx\n", getItem(rd(0,3), 1, 0));
  }
}
