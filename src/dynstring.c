#include <stdio.h>
#include "../head/dynstring.h"

/*******************************************
 *  初始化动态字符串存储容量
 *  pstr:动态字符串存储结构
 *  initsize:字符串初始化分配空间
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
 *  释放动态字符串的内存空间
 *  pstr:动态字符串存储结构
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
 *  重置动态字符串，先释放，重新初始化
 *  pstr:动态字符串存储结构
 ********************************************/
void dynstring_reset(DynString *pstr)
{
    dynstring_free(pstr);
    dynstring_init(pstr, 8); //字符串初始化分配空间8字节
}

/*******************************************
 *  重新分配字符串容量
 *  pstr:动态字符串存储结构
 *  new_size:字符串新长度
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
 *  追加单个字符到动态字符串对象
 *  pstr:动态字符串存储结构
 *  ch:所要追加的字符(原int)
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