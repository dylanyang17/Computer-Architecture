#ifndef REGISTER_STATE_HPP
#define REGISTER_STATE_HPP

#include "reservation_station.hpp"

struct RegisterState {
    // 存储寄存器的值
    int value;
    // 表示当前已经发射的指令中，时间最靠后的写入当前寄存器的指令对应的保留站
    // 若为 NULL 表示当前没有指令会向该寄存器写入。
    ReservationStation* state;

    RegisterState() {
        value = 0;
        state = NULL;
    }
};

#endif // !REGISTER_STATE_HPP