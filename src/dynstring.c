#include <stdio.h>
#include "../head/dynstring.h"

/*******************************************
 *  ��ʼ����̬�ַ����洢����
 *  pstr:��̬�ַ����洢�ṹ
 *  initsize:�ַ�����ʼ������ռ�
 ********************************************/
void dynstring_init(DynString *pstr, int initsize)
{
    if (pstr != NULL)
    {
        pstr->data = (char *)malloc(sizeof(char) * initsize);
        pstr->count = 0;
        pstr->capacity = initsize;
    }
}

/*******************************************
 *  �ͷŶ�̬�ַ������ڴ�ռ�
 *  pstr:��̬�ַ����洢�ṹ
 ********************************************/
void dynstring_free(DynString *pstr)
{
    if (pstr != NULL)
    {
        if (pstr->data)
            free(pstr->data);
        pstr->count = 0;
        pstr->capacity = 0;
    }
}

/*******************************************
 *  ���ö�̬�ַ��������ͷţ����³�ʼ��
 *  pstr:��̬�ַ����洢�ṹ
 ********************************************/
void dynstring_reset(DynString *pstr)
{
    dynstring_free(pstr);
    dynstring_init(pstr, 8); //�ַ�����ʼ������ռ�8�ֽ�
}

/*******************************************
 *  ���·����ַ�������
 *  pstr:��̬�ַ����洢�ṹ
 *  new_size:�ַ����³���
 ********************************************/
void dynstring_realloc(DynString *pstr, int new_size)
{
    int capacity;
    char *data;

    capacity = pstr->capacity;
    while (capacity < new_size)
        capacity = capacity * 2;
    data = realloc(pstr->data, capacity);
    if (!data)
        error("Memory allocation failed");
    pstr->capacity = capacity;
    pstr->data = data;
}

/*******************************************
 *  ׷�ӵ����ַ�����̬�ַ�������
 *  pstr:��̬�ַ����洢�ṹ
 *  ch:��Ҫ׷�ӵ��ַ�(ԭint)
 ********************************************/
void dynstring_chcat(DynString *pstr, char ch)
{
    int count;
    count = pstr->count + 1;
    if (count > pstr->capacity)
        dynstring_realloc(pstr, count);
    ((char *)pstr->data)[count - 1] = ch;
    pstr->count = count;
}