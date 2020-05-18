#ifndef UNIT_STATE_HPP
#define UNIT_STATE_HPP

#include "reservation_station.hpp"

struct UnitState {
    // 该运算部件当前正在执行的指令下标
    int instId;
    // 剩余周期数(等于 0 时表示为最后一个执行周期)
    int countdown;
    // 对应的保留站 (NULL 表示空闲)
    ReservationStation* rs;

    UnitState() {
        rs = NULL;
        instId = -1;
        countdown = -1;
    }
};

#endif // !UNIT_STATE_HPP