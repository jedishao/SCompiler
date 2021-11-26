#pragma once
#include "type.h"
#include "dynstack.h"
#include "utils.h"

Stack global_sym_stack; //ȫ�ַ���ջ
Stack local_sym_stack;  //�ֲ�����ջ

/*���Ŵ洢�ṹ����*/
typedef struct Symbol
{
    int v;                   //���ŵĵ��ʱ���
    int r;                   //���Ź����ļĴ���
    int c;                   //���Ź���ֵ
    Type type;               //������������
    Symbol *next;            //��������������
    Symbol *prev_tok;        //ָ��ǰһ�����ͬ������
}Symbol;

Symbol *sym_direct_push(Stack *ss, int v, Type *type, int c);
Symbol *sym_push(int v, Type *type, int r, int c);
Symbol *func_sym_push(int v, Type *type);
Symbol *var_sym_push(Type *type, int r, int v, int addr);
Symbol *sec_sym_push(char *sec, int c);
void sym_pop(Stack *ptop, Symbol *b);
Symbol *struct_search(int v);
Symbol *sym_search(int v);