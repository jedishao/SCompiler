#pragma once

typedef struct Type
{
    int t;              //数据类型
    struct Symbol *ref; //引用符号
} Type;

/*数据类型编码*/
enum e_TypeCode
{
    T_INT = 0,
    T_CHAR = 1,
    T_SHORT = 2,
    T_VOID = 3,
    T_PTR = 4,
    T_FUNC = 5,
    T_STRUCT = 6,

    T_BTYPE = 0x000f, //基本类型掩码
    T_ARRAY = 0x0010,
};

/*存储类型*/
enum e_StorageClass
{
    SC_GLOBAL = 0x00f0,     //包括整型常量、字符常量、字符串常量、全局变量、函数定义
    SC_LOCAL = 0x00f1,      //栈中变量
    SC_LLOCAL = 0x00f2,     //寄存器溢出存放栈中
    SC_CMP = 0x00f3,        //使用标志寄存器
    SC_VALMASK = 0x00ff,    //存储类型掩码
    SC_LVAL = 0x0100,       //左值
    SC_SYM = 0x0200,        //符号

    SC_ANOM = 0x10000000,   //匿名符号
    SC_STRUCT = 0x20000000, //结构体符号
    SC_MEMBER = 0x40000000, //结构成员变量
    SC_PAPAMS = 0x80000000, //函数参数
};