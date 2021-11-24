#pragma once


/*动态字符串，存储单词*/
typedef struct DynString
{
    int count;    //字符串长度
    int capacity; //包含该字符串的缓冲区长度
    char *data;   //指向字符串的指针
} DynString;

void dynstring_init(DynString *pstr, int initsize);
void dynstring_free(DynString *pstr);
void dynstring_reset(DynString *pstr);
void dynstring_realloc(DynString *pstr, int new_size);
void dynstring_chcat(DynString *pstr, char ch);