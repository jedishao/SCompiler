#pragma once


/*��̬�ַ������洢����*/
typedef struct DynString
{
    int count;    //�ַ�������
    int capacity; //�������ַ����Ļ���������
    char *data;   //ָ���ַ�����ָ��
} DynString;

void dynstring_init(DynString *pstr, int initsize);
void dynstring_free(DynString *pstr);
void dynstring_reset(DynString *pstr);
void dynstring_realloc(DynString *pstr, int new_size);
void dynstring_chcat(DynString *pstr, char ch);