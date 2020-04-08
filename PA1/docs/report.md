# 个人信息和实验环境

## 个人信息

杨雅儒，2017011071，计85。

## 实验环境

* Ubuntu 18.04
* cmake 3.10.2, GNU make 4.1
* gcc 7.5.0

## 编译和运行方法

### 简明

建议直接使用 ``bash run.sh`` 一键编译运行，具体参数会有所提示。

### 详细

在项目目录下存放有文件 ``CMakeLists.txt``，有几种选择：

* 利用 cmake 自行编译；
* 执行 ``bash build.sh`` 一键进行编译；
* 执行 ``bash run.sh <filename> <blockSize> <ways> <strategy> <isWriteAllocate> <isWriteBack>`` 一键编译运行（直接输入``bash run.sh``会显示参数提醒）。

如果使用前两种方法，请接下来**在项目目录**下执行 ``bin/PA1 <filename> <blockSize> <ways> <strategy> <isWriteAllocate> <isWriteBack>`` 运行程序。

## 目录解释

* ``bin/``：存放编译生成的二进制文件；
* ``build/``：构建时生成的目录；
* ``docs/``：存放报告及下发的文件等；
* ``include/``：存放头文件；
* ``src/``：存放源代码文件；
* ``logs/``：存放生成的log信息；
* ``tests/``：存放测试使用的文件，包括数据生成程序及其Makefile；
* ``trace/``：存放 trace 文件。

### logs 目录

其中 ``logs/`` 目录下存放了若干要求格式的Hit/Miss文件（以.txt结尾），以及一个用于汇总信息的 ``log.json`` 文件。

Hit/Miss文件命名形如：``<name>_bs_<blockSize>_ways_<ways>_stra_<strategy>_iwa_<isWriteAllocate>_iwb_<isWriteBack>``

* ``<blockSize>``：块大小，单位为 B；
* ``<ways>``：相联数，取 1 表示直接相联，-1 表示全相联；
* ``<strategy>``：替换策略，0, 1, 2 分别表示LRU、随机替换、二叉树替换；
* ``<isWriteAllocate>``: 是否写分配，true 或者 false；
* ``<isWriteBack>``：是否写回，true 或者 false。

# 程序实现

## 类

主要的类/结构体如下：

* Trace：用于处理输入数据；
* Cache：整个 Cache 对应的类；
* CacheGroup：每个组对应的类；
* Bitset：用于尽量节省空间以模拟硬件情况的类；
* ReplacementStrategy：替换算法的基类，带有虚函数 ``int placeIn(int)``；
* LRUStrategy：继承替换算法基类，实现LRU替换算法；
* RandomStrategy：继承替换算法基类，实现随机替换算法；
* BinaryTreeStrategy：继承替换算法基类，实现二叉树替换算法；

### Bitset 类

由于文档中对实现时使用的结构有要求，这里特地对 Bitset 类进行说明。这个类基础操作上同标准库的 bitset 类类似，只是由于文档有明确要求，便手动实现了一遍 Bitset 类。

Bitset 类中实现了 ``void resize(int)`` 函数，用于初始化这个 Bitset 对象的占用位数，每次会找到最少需要的 char 数目，如下：

```C++
void resize(int bnum) {
    if (data != nullptr) {
      delete[] data;
    }
    this->data = new char[(bnum + 7) / 8];
    this->bnum = bnum;
    memset(data, 0, sizeof(char) * ((bnum + 7) / 8));
}
```

关键即在于 ``(bnum+7) / 8`` 这里。

该类在 Cache 元数据存储、LRU 算法和二叉树替换算法上都得到了使用，统一而高效地利用位运算进行处理，同时很好地模拟了硬件情况。

## 命名空间

为了方便，在 ``utils.hpp`` 中定义了 utils 命名空间，写入了一些工具函数。

# 实验分析

## 不同的 Cache 布局的影响




