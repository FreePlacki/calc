#ifndef OPERATION_H
#define OPERATION_H

enum op_type {
    Add,
    Mul,
    Div,
    Mod,
    Pow,
    Convert,
};

typedef struct {
    enum op_type op_type;
    unsigned short base;
} oper;

#endif
