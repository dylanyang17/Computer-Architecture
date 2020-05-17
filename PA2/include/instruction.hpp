struct Instruction {
    int type, op1, op2, op3;

    Instruction(int _type = 0, int _op1 = 0, int _op2 = 0, int _op3 = 0) {
        type = _type;
        op1 = _op1;
        op2 = _op2;
        op3 = _op3;
    }

    const static int TYPE_ADD = 1, TYPE_SUB = 2, TYPE_MUL = 3, 
        TYPE_DIV = 4, TYPE_LOAD = 5, TYPE_JUMP = 6;
    
    // 判断某个类型是否为功能类型
    static bool isFunctional(int type) {
        return type >= TYPE_ADD && type <= TYPE_DIV;
    }
};