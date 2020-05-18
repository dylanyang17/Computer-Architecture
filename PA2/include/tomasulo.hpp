#ifndef TOMASULO_HPP
#define TOMASULO_HPP

#include <cstdio>
#include <cstring>
#include <vector>
#include <list>
#include "instruction.hpp"
#include "reservation_station.hpp"
#include "register_state.hpp"
#include "unit_state.hpp"
using std::list;
using std::vector;
using std::string;

class Tomasulo {
  public:
    void run(string path) {
        init();
        instructions = Instruction::readFile(path);
        while (pc < instructions.size() || !isAllEmpty()) {
            nextCycle();
        }
    }

  private:
    // 判断是否保留站全空
    bool isAllEmpty() {
        for (int i = 0; i < ars.size(); ++i) {
            if (ars[i].isBusy) return false;
        }
        for (int i = 0; i < mrs.size(); ++i) {
            if (mrs[i].isBusy) return false;
        }
        for (int i = 0; i < lb.size(); ++i) {
            if (lb[i].isBusy) return false;
        }
        return true;
    }

    // 寻找空闲的保留站
    ReservationStation* getRs(Instruction instruction) {
        Instruction::Type type = instruction.type;
        if (Instruction::isAddGroup(type)) {
            // 使用加减法器
            for (int i = 0; i < ars.size(); ++i) {
                if (!ars[i].isBusy) {
                    return &ars[i];
                }
            }
        } else if (Instruction::isMultGroup(type)) {
            // 使用乘除法器
            for (int i = 0; i < mrs.size(); ++i) {
                if (!mrs[i].isBusy) {
                    return &mrs[i];
                }
            }
        } else {
            // 使用 Load 部件
            for (int i = 0; i < lb.size(); ++i) {
                if (!lb[i].isBusy) {
                    return &lb[i];
                }
            }
        }
        return NULL;
    }

    // 对于索引为 r 的寄存器，设置保留站的 v 和 q
    void setVQ(int r, int &v, ReservationStation* &q) {
        if (registerState[r].state == NULL) {
            q = NULL;
            v = registerState[r].value;
        } else {
            q = registerState[r].state;
        }
    }

    // 尝试进行发射 instruction
    void tryIssue(Instruction instruction) {
        if (hasJump || pc >= instructions.size()) return;
        ReservationStation* rs = getRs(instruction);
        if (rs == NULL) return;
        // 有空余的保留站
        rs->isBusy = true;
        rs->type = instruction.type;
        int target = -1;  // 目标寄存器
        if (rs->type == ReservationStation::Type::FUNCTIONAL) {
            FunctionalBuffer* fb = (FunctionalBuffer*) rs;
            if (instruction.type == Instruction::Type::JUMP) {
                // JUMP 指令
                jumpInt1 = instruction.op1;
                jumpInt2 = instruction.op3;
                hasJump = true;
                setVQ(instruction.op2, fb->vj, fb->qj);
            } else {
                // 功能类指令
                setVQ(instruction.op2, fb->vj, fb->qj);
                setVQ(instruction.op3, fb->vk, fb->qk);
                target = instruction.op1;
            }
        } else {
            // Load 指令
            LoadBuffer* b = (LoadBuffer*) rs;
            b->addr = instruction.op2;
            target = instruction.op1;
        }
        if (target != -1) {
            // 修改目标寄存器的状态
            registerState[target].state = rs;
        }
    }

    // 尝试开始执行已经就绪的指令
    void tryExecute() {
        list<ReservationStation*>::iterator it;
        for (it = rsQue.begin(); it != rsQue.end();) {
            if () {
                // TODO: 若就绪且有空余部件
            } else {
                ++it;
            }
        }
    }

    // 进入下一个 cycle
    void nextCycle() {
        ++cycle;
        Instruction instruction = instructions[pc];
        tryIssue(instruction);
        tryExecute();
    }

    // 初始化各变量
    void init() {
        //
        pc = 0;
        cycle = 0;  // 周期从 1 开始计，每次先进行 ++cycle
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
    int jumpInt1, jumpInt2;        // 对应 jump 指令的两个整数
    list<ReservationStation*> rsQue;  // 暂未就绪的 rs 链表
};

#endif // !TOMASULO_HPP