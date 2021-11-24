#pragma once

/*��̬���鶨��*/
typedef struct DynArray
{
    int count;    //��̬����Ԫ�ظ���
    int capacity; //��̬���黺��������
    void **data;  //ָ������ָ������
} DynArray;

void dynarray_realloc(DynArray *parr, int new_size);
void dynarray_add(DynArray *parr, void *data);
void dynarray_init(DynArray *parr, int initsize);
void dynarray_free(DynArray *parr);
int dynarray_search(DynArray *parr, int key);