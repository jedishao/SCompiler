#pragma once
#include "type.h"
#include "dynstack.h"
#include "utils.h"

Stack global_sym_stack; //全局符号栈
Stack local_sym_stack;  //局部符号栈

/*符号存储结构定义*/
typedef struct Symbol
{
    int v;                   //符号的单词编码
    int r;                   //符号关联的寄存器
    int c;                   //符号关联值
    Type type;               //符号数据类型
    Symbol *next;            //关联的其它符号
    Symbol *prev_tok;        //指向前一定义的同名符号
}Symbol;

Symbol *sym_direct_push(Stack *ss, int v, Type *type, int c);
Symbol *sym_push(int v, Type *type, int r, int c);
Symbol *func_sym_push(int v, Type *type);
Symbol *var_sym_push(Type *type, int r, int v, int addr);
Symbol *sec_sym_push(char *sec, int c);
void sym_pop(Stack *ptop, Symbol *b);
Symbol *struct_search(int v);
Symbol *sym_search(int v);