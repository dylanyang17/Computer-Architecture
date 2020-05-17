#include <string>
#include <cassert>

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

    Instruction(std::string s) {
        int last = -1;          // 上一个逗号的索引
        int pos = s.find(',');  // 当前逗号的索引
        std::string typeStr = s.substr(last+1, pos-last-1);
        this->type = stringToType(typeStr);
        last = pos;
        pos = s.find(',', last+1);
        std::string opStr1 = s.substr(last+1, pos-last-1);
        this->op1 = stringToOp(opStr1, isFunctional(type) || type == Type::LOAD);
        if (type == Type::LOAD) {
            std::string opStr2 = s.substr(pos+1);
            this->op2 = stringToOp(opStr2, false);
        } else {
            last = pos;
            pos = s.find(',', last+1);
            std::string opStr2 = s.substr(last+1, pos-last-1);
            this->op2 = stringToOp(opStr2, true);
            std::string opStr3 = s.substr(pos+1);
            this->op3 = stringToOp(opStr3, isFunctional(type));
        }
    }

    // 从 string 类型转换为 op, isRegister 为 true 表示为寄存器否则为十六进制值
    static int stringToOp(std::string s, bool isRegister) {
        if (isRegister) {
            return atoi(s.substr(1).c_str());
        } else {
            int ret;
            sscanf(s.c_str(), "%x", &ret);
            return ret;
        }
    }

    // 从 string 类型转换为 Type
    static Type stringToType(std::string s) {
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

    // 判断某个类型是否为功能类型
    static bool isFunctional(Type type) {
        return type == Type::ADD || type == Type::SUB
            || type == Type::MUL || type == Type::DIV;
    }
};