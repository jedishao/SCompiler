#include <stdio.h>
#include <stdlib.h>
#include "../head/symbol.h"

/*******************************************
  * 功能: 将符号放在符号栈
  * v:    符号编号
  * type: 符号数据类型
  * c:    符号关联值
 ********************************************/
Symbol *sym_direct_push(Stack *ss, int v, Type *type, int c)
{
    Symbol s, *p;
    s.v = v;
    s.type.t = type->t;
    s.type.ref = type->ref;
    s.c = c;
    s.next = NULL;
    p = (Symbol *)stack_push(ss, &s, sizeof(Symbol));
    return p;
}

/*******************************************
  * 功能: 将符号放在符号栈中, 动态判断是放入全局符号栈还是局部符号栈
  * v:    符号编号
  * type: 符号数据类型
  * r:    符号存储类型
  * c:    符号关联值
 ********************************************/
Symbol *sym_push(int v, Type *type, int r, int c)
{
    Symbol *ps, **pps;
    TKWord *ts;
    Stack *ss;

    if (stack_is_empty(&local_sym_stack) == 0)
    {
        ss = &local_sym_stack;
    }
    else
    {
        ss = &global_sym_stack;
    }
    ps = sym_direct_push(ss, v, type, c);
    ps->r = r;

    //don't record struct and anonymous symbol
    if ((v & SC_STRUCT) || v < SC_ANOM)
    {
        //update sym_struct or sym_identifier
        ts = (TKWord *)tktable.data[(v & ~SC_STRUCT)];
        if (v & SC_STRUCT)
            pps = &ts->sym_struct;
        else
            pps = &ts->sym_identifier;
        ps->prev_tok = *pps;
        *pps = ps;
    }
    return ps;
}

/*******************************************
  * 功能: 将函数符号放入全局符号表中
  * v:    符号编号
  * type: 符号数据类型
 ********************************************/
Symbol *func_sym_push(int v, Type *type)
{
    Symbol *s, **ps;
    s = sym_direct_push(&global_sym_stack, v, type, 0);

    ps = &((TKWord *)tktable.data[v])->sym_identifier;
    //同名函数符号, 函数符号放在最后->->...s
    while (*ps != NULL)
        ps = &(*ps)->prev_tok;
    s->prev_tok = NULL;
    *ps = s;
    return s;
}