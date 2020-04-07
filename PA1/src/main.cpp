#include <trace.hpp>
#include <cstdio>

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("Usage: bash run.sh trace/<name>.trace\n");
    return 0;
  }
  char *filename = argv[1];
  Trace *trace = new Trace();
  FILE *file = fopen(filename, "r");
  trace->readItems(file);
  delete trace;
  return 0;
}