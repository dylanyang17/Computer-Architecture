#ifndef TOMASULO_HPP
#define TOMASULO_HPP

#include <cstdio>
#include <cstring>
#include <vector>
#include "instruction.hpp"
#include "reservation_station.hpp"
#include "register_state.hpp"
#include "unit_state.hpp"
using std::vector;
using std::string;

class Tomasulo {
  public:
    void run(string path) {
        init();
        instructions = Instruction::readFile(path);
        while (pc < instructions.size()) {
          nextCycle();
        }
    }

  private:
    // 尝试进行发射 instruction
    void tryIssue(Instruction instruction) {
        if (hasJump) return;
        
    }

    // 执行当前 cycle 并时间推移到下一个 cycle
    void nextCycle() {
        Instruction instruction = instructions[pc];
        tryIssue(instruction);
    }

    // 初始化各变量
    void init() {
        //
        pc = 0;
        cycle = 1;  // 周期从 1 开始计
        hasJump = false;
        ars.clear();
        mrs.clear();
        lb.clear();
        unitAdd.clear();
        unitMult.clear();
        unitLoad.clear();
        // 6 个加减法保留站
        for (int i = 0; i < 6; ++i) ars.push_back(FunctionalBuffer());
        // 3 个乘除法保留站
        for (int i = 0; i < 3; ++i) mrs.push_back(FunctionalBuffer());
        // 3 个 LoadBuffer
        for (int i = 0; i < 3; ++i) lb.push_back(LoadBuffer());
        // 3 个加减法器
        for (int i = 0; i < 3; ++i) unitAdd.push_back(UnitState());
        // 2 个乘除法器
        for (int i = 0; i < 2; ++i) unitMult.push_back(UnitState());
        // 2 个 Load 部件
        for (int i = 0; i < 2; ++i) unitLoad.push_back(UnitState());
        
    }

    vector<Instruction> instructions;
    int pc;        // 下一条发射的指令的索引
    int cycle;     // 当前的周期数
    bool hasJump;  // 当前是否有 jump 指令被发射
    RegisterState registerState[32];
    vector<FunctionalBuffer> ars;  // 加减法保留站
    vector<FunctionalBuffer> mrs;  // 乘除法保留站
    vector<LoadBuffer> lb;         // LoadBuffer
    vector<UnitState> unitAdd;     // 加减法器
    vector<UnitState> unitMult;    // 乘除法器
    vector<UnitState> unitLoad;    // Load 部件   
};

#endif // !TOMASULO_HPP