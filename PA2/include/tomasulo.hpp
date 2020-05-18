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
    ReservationStation* findEmptyRs(Instruction instruction) {
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

    // 尝试对 VQ 进行写回
    bool tryWriteVQ(ReservationStation *from, int value, int &v, ReservationStation* &q) {
        if (q == from) {
            q = NULL;
            v = value;
            return true;
        } else return false;
    }

    // 尝试进行发射 instruction
    bool tryIssue() {
        Instruction instruction = instructions[pc];
        if (hasJump || pc >= instructions.size()) return false;
        ReservationStation* rs = findEmptyRs(instruction);
        if (rs == NULL) return false;
        // 有空余的保留站
        rs->isBusy = true;
        rs->instId = pc;
        int target = -1;  // 目标寄存器
        if (rs->type == ReservationStation::Type::FUNCTIONAL) {
            FunctionalBuffer* fb = (FunctionalBuffer*) rs;
            if (instruction.type == Instruction::Type::JUMP) {
                // JUMP 指令
                jumpInt1 = instruction.op1;
                jumpInt2 = instruction.op3;
                hasJump = true;
                setVQ(instruction.op2, fb->vj, fb->qj);
                fb->qk = NULL;
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
        ++pc;
        return true;
    }

    // 寻找空闲的 unit
    int findEmptyUnit(vector<UnitState>& units) {
        int ret = -1;
        for (int i = 0; i < units.size(); ++i) {
            if (units[i].rs == NULL) {
                ret = i;
                break;
            }
        }
        return ret;
    }

    bool tryExecuteOne(ReservationStation* rs) {
        UnitState *unit;
        if (rs->type == ReservationStation::Type::FUNCTIONAL) {
            FunctionalBuffer* fb = (FunctionalBuffer*) rs;
            if (Instruction::isAddGroup(fb->op)) {
                // 使用加减法器
                int pos = findEmptyUnit(unitAdd);
                if (pos == -1) return false;
                unit = &unitAdd[pos];
                unit->rs = rs;
                unit->instId = rs->instId;
                unit->countdown = Instruction::getCycleNeeded(fb->op);
            } else if (Instruction::isMultGroup(fb->op)) {
                // 使用乘除法器
                int pos = findEmptyUnit(unitMult);
                if (pos == -1) return false;
                unit = &unitMult[pos];
                unit->rs = rs;
                unit->instId = rs->instId;
                unit->countdown = Instruction::getCycleNeeded(fb->op);
                if (fb->op == Instruction::Type::DIV && fb->vk == 0) {
                    // 特殊处理除零的情况
                    fb->vk = 1;
                    unit->countdown = 1;
                }
            }
        } else {
            // 使用 Load 部件
            LoadBuffer* b = (LoadBuffer*) rs;
            int pos = findEmptyUnit(unitLoad);
            if (pos == -1) return false;
            unit = &unitLoad[pos];
            unit->rs = rs;
            unit->instId = rs->instId;
            unit->countdown = Instruction::getCycleNeeded(Instruction::Type::LOAD);
        }
        return true;
    }

    // 尝试开始执行已经就绪的指令
    void tryExecute() {
        list<ReservationStation*>::iterator it;
        for (it = rsQue.begin(); it != rsQue.end();) {
            if (tryExecuteOne(*it)) {
                // 若加入执行成功
                it = rsQue.erase(it);
            } else {
                ++it;
            }
        }
    }

    // 尝试对 unit 进行写回，若 countdown 非零则仅减 1
    bool tryWriteBackOne(UnitState *unit) {
        if (unit->rs == NULL) return false;
        if (unit->countdown > 0) {
            unit->countdown--;
            return false;
        } else {
            // 可进行写回操作
            int res;
            if (unit->rs->type == ReservationStation::Type::FUNCTIONAL) {
                FunctionalBuffer* fb = (FunctionalBuffer*) unit->rs;
            } else {
                LoadBuffer* b = (LoadBuffer*) unit->rs;
                res = b->addr;
            }

            // 判断有无需要该值的保留站
            for (int i = 0; i < ars.size(); ++i) {
                tryWriteVQ(unit->rs, res, ars[i].vj, ars[i].qj);
                tryWriteVQ(unit->rs, res, ars[i].vk, ars[i].qk);
            }
            for (int i = 0; i < mrs.size(); ++i) {
                tryWriteVQ(unit->rs, res, mrs[i].vj, mrs[i].qj);
                tryWriteVQ(unit->rs, res, mrs[i].vk, mrs[i].qk);
            }

            // 判断是否写入寄存器栈
            for (int i = 0; i < 32; ++i) {
                if (registerState[i].state == unit->rs) {
                    registerState[i].value = res;
                }
            }

            unit->rs->isBusy = false;
            unit->rs = NULL;
            return true;
        }
    }

    // 尝试进行写回
    void tryWriteBack() {
        for (int i = 0; i < unitAdd.size(); ++i) {
            tryWriteBackOne(&unitAdd[i]);
        }
        for (int i = 0; i < unitMult.size(); ++i) {
            tryWriteBackOne(&unitMult[i]); 
        }
        for (int i = 0; i < unitLoad.size(); ++i) {
            tryWriteBackOne(&unitLoad[i]);
        }
    }

    // 进入下一个 cycle
    void nextCycle() {
        ++cycle;
        printf("正在执行 pc %d, cycle %d...\n", pc, cycle);
        tryWriteBack();
        tryIssue();
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