# 注意事项

* 首先要注意除法在除零的时候只需要 1 个周期，而正常情况下需要 4 个周期；
* 当两条指令同时就绪时，**取时间上更靠前的指令先进入功能部件执行**，注意如果有两个对应功能部件同时空闲，则可以同时分别进入执行；
* 当两条指令同时执行完毕时，可同时写回（尽管这一点我认为使用公共总线的话这样不大合理，但例子中确实是这么给的，为统一就按照例子中的来）
* 同一周期中不同部分重叠时，依据例子，按照 ``写回->取消占用保留站->发射并占用保留站`` 的顺序来处理（似乎也不大合理）；
* 存在已发射的 JUMP 指令时不继续进行发射。

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

* ReservationStation 基类：存储保留站类型 type 以及是否忙碌 isBusy；
* FunctionalBuffer 派生类：用于加法、乘法等功能部件的保留站，存储 Instruction::Type 类型变量 op，整型变量 vj 和 vk 表示两个源操作数的值，ReservationStation\* 类型变量 qj 和 qk 表示两个源操作数的来源保留站，注意 v/q 只有一个会生效，q 为 NULL 表示已准备好，此时应当用 v；
* LoadBuffer 派生类：用于 Load 的保留站，存储整型变量 addr 表示要 load 的值。

存储先进先出的队列 rsQue，用于实现先发射的先执行。

## 寄存器状态

每个寄存器用一个结构体 RegisterState 存储，包含：

* 整型变量 value 表示寄存器的值；
* ReservationStation\* 类型变量 state 表示当前已经发射的指令中，时间最靠后的写入当前寄存器的指令对应的保留站，若为 NULL 表示当前发射的指令中没有要向该寄存器写入的。

## 运算部件状态

每个运算部件（包括功能运算部件和Load运算部件）用一个结构体 UnitState 存储，包含：

* 整型变量 instId 表示该运算部件当前正在执行的指令下标（-1 表示该运算部件空闲）；
* 整型变量 countdown 表示剩余周期数；
* ReservationStation\* 类型变量 rs 表示对应的保留站。

不同类型的运算部件使用不同的数组来区分即可，例如 unitStateAdd[3], unitStateMult[2], unitStateLoad[2]。
