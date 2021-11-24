#pragma once

/*动态数组定义*/
typedef struct DynArray
{
    int count;    //动态数组元素个数
    int capacity; //动态数组缓冲区长度
    void **data;  //指向数据指针数组
} DynArray;

void dynarray_realloc(DynArray *parr, int new_size);
void dynarray_add(DynArray *parr, void *data);
void dynarray_init(DynArray *parr, int initsize);
void dynarray_free(DynArray *parr);
int dynarray_search(DynArray *parr, int key);