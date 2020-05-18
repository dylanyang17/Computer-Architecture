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
using std::to_string;

class Tomasulo {
  public:
    void run(string path, bool displayOn) {
        init();
        this->displayOn = displayOn;
        instructions = Instruction::readFile(path);
        int size = instructions.size();
        issueCycle.reserve(size);
        doneCycle.reserve(size);
        wbCycle.reserve(size);
        for (int i = 0; i < size; ++i) {
            issueCycle.push_back(-1);
            doneCycle.push_back(-1);
            wbCycle.push_back(-1);
        }
        while (pc < instructions.size() || !isAllEmpty()) {
            nextCycle();
            if (displayOn) printNowState();
        }
        int lpos = path.find_last_of("/")+1, rpos = path.find_last_of(".")-1;
        string filename = path.substr(lpos, rpos-lpos+1);
        FILE *file = fopen((string("logs/2017011071_") + filename + string(".log")).c_str(), "w");
        for (int i = 0; i < instructions.size(); ++i) {
            fprintf(file, "%d %d %d\n", issueCycle[i], doneCycle[i], wbCycle[i]);
        }
        fclose(file);
    }

  private:
    void printUnit() {
        printf("Units:\n");
        for (int i = 0; i < unitAdd.size(); ++i) {
            if (unitAdd[i].rs != NULL) {
                printf("unitAdd%d:: instId: %d  countdown: %d\n", i+1, unitAdd[i].instId ,unitAdd[i].countdown);
            }
        }
        for (int i = 0; i < unitMult.size(); ++i) {
            if (unitMult[i].rs != NULL) {
                printf("unitMult%d:: instId: %d  countdown: %d\n", i+1, unitMult[i].instId ,unitMult[i].countdown);
            }
        }
        for (int i = 0; i < unitLoad.size(); ++i) {
            if (unitLoad[i].rs != NULL) {
                printf("unitLoad%d:: instId: %d  countdown: %d\n", i+1, unitLoad[i].instId ,unitLoad[i].countdown);
            }
        }
        printf("\n");
    }

    void printLoadBuffer() {
        printf("Load Buffer:\n");
        for (int i = 0; i < lb.size(); ++i) {
            printf("%d:: Busy: %s  Address: %d\n", i, (lb[i].isBusy ? "True" : "False"),
                lb[i].addr);
        }
        printf("\n");
    }

    // 保证为 FunctionalBuffer
    void printRSOne(FunctionalBuffer *fb) {
        printf("%s:: Busy: %s  Op: %s  Vj: %s  Vk: %s  Qj: %s  Qk: %s\n",
                fb->name.c_str(),
                (fb->isBusy ? "True" : "False"),
                Instruction::typeToString(fb->op).c_str(),
                ((fb->qj == NULL) ? to_string(fb->vj).c_str() : "None"),
                ((fb->qk == NULL && fb->op!=Instruction::Type::JUMP) ? to_string(fb->vk).c_str() : "None"),
                ((fb->qj == NULL) ? "None" : fb->qj->name.c_str()),
                ((fb->qk == NULL) ? "None" : fb->qk->name.c_str()));
    }

    void printRS() {
        printf("Reservation Station:\n");
        for (int i = 0; i < ars.size(); ++i) {
            printRSOne(&ars[i]);
        }
        for (int i = 0; i < mrs.size(); ++i) {
            printRSOne(&mrs[i]);
        }
        printf("\n");
    }

    void printRegister() {
        printf("Register:\n");
        for (int i = 0; i < 32; ++i) {
            RegisterState *reg = &registerState[i];
            printf("%d:: value: %s\n", i, 
                ((reg->state == NULL) ? to_string(reg->value).c_str() : reg->state->name.c_str()));
        }
        printf("\n");
    }

    // 打印瞬时状态
    void printNowState() {
        printf("打印 cycle %d 的瞬时状态...\n", cycle);
        printLoadBuffer();
        printUnit();
        printRS();
        printRegister();
        printf("\n");
    }

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
            fb->op = instruction.type;
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
        rsQue.push_back(rs);
        if (issueCycle[pc] == -1) issueCycle[pc] = cycle;
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
            if ((*it)->isReady() && tryExecuteOne(*it)) {
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
            bool wb = true;
            if (unit->rs->type == ReservationStation::Type::FUNCTIONAL) {
                FunctionalBuffer* fb = (FunctionalBuffer*) unit->rs;
                if (fb->op == Instruction::Type::ADD) {
                    res = fb->vj + fb->vk;
                } else if (fb->op == Instruction::Type::SUB) {
                    res = fb->vj - fb->vk;
                } else if (fb->op == Instruction::Type::MUL) {
                    res = fb->vj * fb->vk;
                } else if (fb->op == Instruction::Type::DIV) {
                    res = fb->vj / fb->vk;
                } else if (fb->op == Instruction::Type::JUMP) {
                    if (fb->vj == jumpInt1) {
                        pc += jumpInt2 - 1;
                    }
                    hasJump = false;
                    wb = false;
                }
            } else {
                LoadBuffer* b = (LoadBuffer*) unit->rs;
                res = b->addr;
            }

            if (wb) {
                // 判断有无需要该值的保留站
                for (int i = 0; i < ars.size(); ++i) {
                    if (!ars[i].isBusy) continue;
                    tryWriteVQ(unit->rs, res, ars[i].vj, ars[i].qj);
                    tryWriteVQ(unit->rs, res, ars[i].vk, ars[i].qk);
                }
                for (int i = 0; i < mrs.size(); ++i) {
                    if (!mrs[i].isBusy) continue;
                    tryWriteVQ(unit->rs, res, mrs[i].vj, mrs[i].qj);
                    tryWriteVQ(unit->rs, res, mrs[i].vk, mrs[i].qk);
                }

                // 判断是否写入寄存器栈
                for (int i = 0; i < 32; ++i) {
                    if (registerState[i].state == unit->rs) {
                        registerState[i].value = res;
                        registerState[i].state = NULL;
                    }
                }
            }

            if (unit->rs->type == ReservationStation::Type::FUNCTIONAL) {
                FunctionalBuffer* fb = (FunctionalBuffer*) unit->rs;
                fb->qj = NULL;
                fb->qk = NULL;
            }
            if (doneCycle[unit->rs->instId] == -1) {
                doneCycle[unit->rs->instId] = cycle-1;
                wbCycle[unit->rs->instId] = cycle;
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
        if (displayOn) printf("正在执行 pc %d, cycle %d...\n", pc, cycle);
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
        for (int i = 0; i < 6; ++i) ars.push_back(FunctionalBuffer(string("Ars") + to_string(i+1)));
        // 3 个乘除法保留站
        for (int i = 0; i < 3; ++i) mrs.push_back(FunctionalBuffer(string("Mrs") + to_string(i+1)));
        // 3 个 LoadBuffer
        for (int i = 0; i < 3; ++i) lb.push_back(LoadBuffer(string("LB") + to_string(i+1)));
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

    vector<int> issueCycle, doneCycle, wbCycle;
    bool displayOn;
};

#endif // !TOMASULO_HPP