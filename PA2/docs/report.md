# 设计

## 指令

定义 Instruction 类表示指令，type 表示指令类型, 整型变量 op1, op2, op3 分为表示三个操作数，如果某操作数为 REGISTER，则存储其编号，若为 INTEGER 则直接存储其值。类型分类如下：

* ADD 类型；
* SUB 类型；
* MUL 类型；
* DIV 类型；
* LOAD 类型；
* JUMP 类型。

## 保留站

为了方便统一，这里将课件中的保留站和 Load Buffer 统一认为为广义的保留站，而将狭义的保留站命名为 Functional Buffer。

* ReservationStation 基类：存储保留站类型 type 以及是否忙碌 busy；
* FunctionalBuffer 派生类：存储用于加法、乘法等功能部件的保留站；
* LoadBuffer 派生类：存储用于 Load 的保留站。

## 
