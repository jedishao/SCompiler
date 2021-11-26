#include <stdio.h>
#include <stdlib.h>
#include "../head/symbol.h"

/*******************************************
  * ����: �����ŷ��ڷ���ջ
  * v:    ���ű��
  * type: ������������
  * c:    ���Ź���ֵ
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
  * ����: �����ŷ��ڷ���ջ��, ��̬�ж��Ƿ���ȫ�ַ���ջ���Ǿֲ�����ջ
  * v:    ���ű��
  * type: ������������
  * r:    ���Ŵ洢����
  * c:    ���Ź���ֵ
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
  * ����: ���������ŷ���ȫ�ַ��ű���
  * v:    ���ű��
  * type: ������������
 ********************************************/
Symbol *func_sym_push(int v, Type *type)
{
    Symbol *s, **ps;
    s = sym_direct_push(&global_sym_stack, v, type, 0);

    ps = &((TKWord *)tktable.data[v])->sym_identifier;
    //ͬ����������, �������ŷ������->->...s
    while (*ps != NULL)
        ps = &(*ps)->prev_tok;
    s->prev_tok = NULL;
    *ps = s;
    return s;
}