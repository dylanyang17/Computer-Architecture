#ifndef TOMASULO_HPP
#define TOMASULO_HPP

#include <cstdio>
#include <cstring>
#include <vector>
#include "instruction.hpp"
using std::vector;
using std::string;

class Tomasulo {
  public:

    void run(string path) {
        instructions = Instruction::readFile(path);
    }

  private:
    vector<Instruction> instructions;
    int pc;  // 下一条发射的指令的索引
};

#endif // !TOMASULO_HPP