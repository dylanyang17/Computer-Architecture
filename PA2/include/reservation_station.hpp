#ifndef RESERVATION_STATION_HPP
#define RESERVATION_STATION_HPP

#include "instruction.hpp"

class ReservationStation {
  public:
    enum class Type {
        FUNCTIONAL = 1,
        LOAD = 2
    };

    ReservationStation() {
        this->isBusy = false;
    }

    ReservationStation(Type _type) {
        this->type = _type;
        this->isBusy = false;
    }

    Type type;      // 类型，用于确定是 Functional 还是 Load
    bool isBusy;    // 是否包含指令
    int issueTime;  // 发射该条指令时的时钟周期数
};

class FunctionalBuffer : public ReservationStation {
  public:
    FunctionalBuffer() : ReservationStation(Type::FUNCTIONAL) {
        // 注意 isBusy 将会被赋值为 false，所以其余初值无关紧要
    }
    
    Instruction::Type op;        // 操作类型
    int vj, vk;                  // 源操作数的值
    ReservationStation* qj, qk;  // 源操作数的来源保留站
};

class LoadBuffer : public ReservationStation {
  public:
    LoadBuffer() : ReservationStation(Type::LOAD) {
        // 注意 isBusy 将会被赋值为 false，所以其余初值无关紧要
    }
    
    int addr;  // 存储要 load 的值
};

#endif // !RESERVATION_STATION_HPP