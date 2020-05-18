#include <cstdio>
#include "tomasulo.hpp"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: bin/PA2 TestCase/<name>.nel 0/1\n");
        return 0;
    }
    int tmp;
    sscanf(argv[2], "%d", &tmp);
    Tomasulo tomasulo;
    tomasulo.run(argv[1], tmp);
    return 0;
}