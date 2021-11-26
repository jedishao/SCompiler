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

Symbol *var_sym_push(Type *type, int r, int v, int addr)
{
    Symbol *sym = NULL;
    if ((r & SC_VALMASK) == SC_LOCAL)
    {
        sym = sym_push(v, type, r, addr);
    }
    else if (v && (r & SC_VALMASK) == SC_GLOBAL)
    {
        sym = sym_search(v);
        if (sym)
            error("%s redefine\n", ((TKWord *)tktable.data[v])->spelling);
        else
            sym = sym_push(v, type, r | SC_SYM, 0);
    }
    //else 字符串常量符号
    return sym;
}

/*******************************************
  * 功能: 将节名称放入全局符号表
  * sec:  节名称
  * c:    符号关联值
 ********************************************/
Symbol *sec_sym_push(char *sec, int c)
{
    TKWord *tp;
    Symbol *s;
    Type type;
    type.t = T_INT;
    tp = tkword_insert(sec);
    token = tp->tkcode;
    s = sym_push(token, &token, SC_GLOBAL, c);
    return s;
}

/*******************************************
  * 功能: 弹出栈中符号直到栈顶符号为'b'
  * ptop: 符号栈栈顶
  * b:    符号指针
 ********************************************/
void sym_pop(Stack *ptop, Symbol *b)
{
    Symbol *s, **ps;
    TKWord *ts;
    int v;

    s = (Symbol *)stack_get_top(ptop);
    while (s != b)
    {
        v = s->v;
        // update sym_struct sym_identifier
        if ((v & SC_STRUCT) || v < SC_ANOM)
        {
            ts = (TKWord *)tktable.data[(v & ~SC_STRUCT)];
            if (v & SC_STRUCT)
                ps = &ts->sym_struct;
            else
                ps = &ts->sym_identifier;
            *ps = s->prev_tok;
        }
        stack_pop(ptop);
        s = (Symbol *)stack_get_top(ptop);
    }
}

/*******************************************
  * 功能: 查找结构定义
  * v:    符号编号
 ********************************************/
Symbol *struct_search(int v)
{
    if (v >= tktable.count)
        return NULL;
    else
        return ((TKWord *)tktable.data[v])->sym_struct;
}

/*******************************************
  * 功能: 查找标识符定义
  * v:    符号编号
 ********************************************/
Symbol *sym_search(int v)
{
    if (v >= tktable.count)
        return NULL;
    else
        return ((TKWord *)tktable.data[v])->sym_identifier;
}