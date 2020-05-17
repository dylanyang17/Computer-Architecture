# 设计

## 指令

定义结构体 Instruction 表示指令，type 表示指令类型, 整型变量 op1, op2, op3 分为表示三个操作数，如果某操作数为 REGISTER，则存储其编号，若为 INTEGER 则直接存储其值。类型分类如下：

* ADD 类型；
* SUB 类型；
* MUL 类型；
* DIV 类型；
* LOAD 类型；
* JUMP 类型。

指令使用一个 ``vector<Instruction> instructions`` 存储下来，下标从 0 开始，pc 为下一个即将发射的指令下标。

## 保留站

为了方便统一，这里将课件中的保留站和 Load Buffer 统一认为为广义的保留站，而将狭义的保留站命名为 Functional Buffer。

* ReservationStation 基类：存储保留站类型 type 以及是否忙碌 busy；
* FunctionalBuffer 派生类：存储用于加法、乘法等功能部件的保留站；
* LoadBuffer 派生类：存储用于 Load 的保留站。

## 寄存器状态

每个寄存器用一个结构体 RegisterState 存储，包含整型变量 value 表示寄存器的值，以及 ReservationStation\* 类型的 state 表示当前已经发射的指令中，时间最靠后的写入当前寄存器的指令对应的保留站，若为 NULL 表示当前发射的指令中没有要向该寄存器写入的。

## 运算部件状态

每个运算部件（包括功能运算部件和Load运算部件）用一个结构体 UnitState 存储，包含整型变量 instId 表示该运算部件当前正在执行的指令下标（-1 表示该运算部件空闲），以及整型变量 countdown 表示剩余周期数。

不同类型的运算部件使用不同的数组来区分即可，例如 unitStateAdd[3], unitStateMult[2], unitStateLoad[2]。


