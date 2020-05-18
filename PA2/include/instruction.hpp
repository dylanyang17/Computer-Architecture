#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <vector>
#include <cassert>
using std::vector;
using std::string;

struct Instruction {
    enum class Type {
        ADD = 1,
        SUB = 2,
        MUL = 3,
        DIV = 4,
        LOAD = 5,
        JUMP = 6
    };

    int op1, op2, op3;
    Type type;

    Instruction(Type _type, int _op1 = 0, int _op2 = 0, int _op3 = 0) {
        type = _type;
        op1 = _op1;
        op2 = _op2;
        op3 = _op3;
    }

    Instruction(string s) {
        int last = -1;          // 上一个逗号的索引
        int pos = s.find(',');  // 当前逗号的索引
        string typeStr = s.substr(last+1, pos-last-1);
        this->type = stringToType(typeStr);
        last = pos;
        pos = s.find(',', last+1);
        string opStr1 = s.substr(last+1, pos-last-1);
        this->op1 = stringToOp(opStr1, isOPR(type) || type == Type::LOAD);
        if (type == Type::LOAD) {
            string opStr2 = s.substr(pos+1);
            this->op2 = stringToOp(opStr2, false);
        } else {
            last = pos;
            pos = s.find(',', last+1);
            string opStr2 = s.substr(last+1, pos-last-1);
            this->op2 = stringToOp(opStr2, true);
            string opStr3 = s.substr(pos+1);
            this->op3 = stringToOp(opStr3, isOPR(type));
        }
    }

    bool operator==(const Instruction b) const {
        if (this->type != b.type) return false;
        if (this->type == Type::LOAD) {
            return this->op1 == b.op1 && this->op2 == b.op2;
        } else {
            return this->op1 == b.op1 && this->op2 == b.op2 && this->op3 == b.op3;
        }
    }

    // 读取文件得到指令
    static vector<Instruction> readFile(string path) {
        char tmp[100];
        FILE* file = fopen(path.c_str(), "r");
        vector<Instruction> ret;
        while(!feof(file)) {
            fgets(tmp, 100, file);
            string s = tmp;
            ret.push_back(Instruction(s));
        }
        fclose(file);
        return ret;
    }

    // 从 string 类型转换为 op, isRegister 为 true 表示为寄存器否则为十六进制值
    static int stringToOp(string s, bool isRegister) {
        if (isRegister) {
            return atoi(s.substr(1).c_str());
        } else {
            int ret;
            sscanf(s.c_str(), "%x", &ret);
            return ret;
        }
    }

    // 从 string 类型转换为 Type
    static Type stringToType(string s) {
        if (s == "ADD") {
            return Type::ADD;
        } else if (s == "SUB") {
            return Type::SUB;
        } else if (s == "MUL") {
            return Type::MUL;
        } else if (s == "DIV") {
            return Type::DIV;
        } else if (s == "LD") {
            return Type::LOAD;
        } else if (s == "JUMP") {
            return Type::JUMP;
        } else {
            assert(0);
        }
    }

    static string typeToString(Type type) {
        if (type == Type::ADD) {
            return string("ADD");
        } else if (type == Type::SUB) {
            return string("SUB");
        } else if (type == Type::MUL) {
            return string("MUL");
        } else if (type == Type::DIV) {
            return string("DIV");
        } else if (type == Type::LOAD) {
            return string("LD");
        } else if (type == Type::JUMP) {
            return string("JUMP");
        } else {
            return string("None");
        }
    }

    // 获得指令需要执行的周期数
    static int getCycleNeeded(Type type) {
        if (type == Type::ADD) {
            return 3;
        } else if (type == Type::SUB) {
            return 3;
        } else if (type == Type::MUL) {
            return 4;
        } else if (type == Type::DIV) {
            return 4;
        } else if (type == Type::LOAD) {
            return 3;
        } else if (type == Type::JUMP) {
            return 1;
        } else {
            assert(0);
        }
    }

    // 判断某个类型是否为 OPR 类型
    static bool isOPR(Type type) {
        return type == Type::ADD || type == Type::SUB
            || type == Type::MUL || type == Type::DIV;
    }

    // 判断某个类型是否使用加减法器
    static bool isAddGroup(Type type) {
        return type == Type::ADD || type == Type::SUB || type == Type::JUMP;
    }

    // 判断某个类型是否使用乘除法器
    static bool isMultGroup(Type type) {
        return type == Type::MUL || type == Type::DIV;
    }

    // 判断某个类型是否使用 Load 部件
    static bool isLoadGroup(Type type) {
        return type == Type::LOAD;
    }
};

#endif // !INSTRUCTION_HPP