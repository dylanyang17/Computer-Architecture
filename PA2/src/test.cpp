#include <cstdio>
#include <cstring>
#include <vector>
#include "instruction.hpp"
#include "test.hpp"
using std::vector;
using std::string;

// 用于测试的函数，release 版本不应调用此函数
void test() {
    printf("测试中...\n");
    vector<Instruction> instructions = Instruction::readFile("TestCase/Fact.nel");
    assert(instructions.size() == 10);
    assert(instructions[2] == Instruction(Instruction::Type::LOAD, 6, 0x1E));
    assert(instructions[4] == Instruction(Instruction::Type::MUL, 0, 0, 1));
    assert(instructions[5] == Instruction(Instruction::Type::SUB, 3, 2, 1));
    assert(instructions[6] == Instruction(Instruction::Type::ADD, 1, 4, 1));
    assert(instructions[7] == Instruction(Instruction::Type::JUMP, 0x0, 3, 0x2));
    assert(instructions[7] == Instruction(Instruction::Type::JUMP, 0x0, 31, 0xFFFFFFFC));
    printf("通过测试, 开始运行...\n");
}