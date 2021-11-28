#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <Windows.h>
#include "dynarray.h"
#include "dynstring.h"

/*单词存储结构定义*/
typedef struct TKWord
{
    int tkcode;                    //单词编码
    struct TKWord *next;           //指向哈希冲突的同义词
    char *spelling;                //单词字符串
    struct Symbol *sym_struct;     //指向单词所表示的结构定义
    struct Symbol *sym_identifier; //指向单词所表示的标识符
} TKWord;

#define MAXKEY 1024           //哈希表容量
TKWord *tk_hashtable[MAXKEY]; //单词哈希表
DynArray tktable;             //单词表

int token;
char ch;
FILE *fin;

char *filename;
int line_num;

DynString tkstr, sourcestr; //单词字符串, 单词源码字符串
int tkvalue;

/*单词编码*/
enum e_Token_Code
{
    /*运算符及分隔符*/
    TK_PLUS,      //+加号
    TK_MINUS,     //-减号
    TK_STAR,      //*星号
    TK_DIVIDE,    ///除号
    TK_MOD,       //%求余运算符
    TK_EQ,        //==等于号
    TK_NEQ,       //！=不等于
    TK_LT,        //<小于号
    TK_LEQ,       //<=小于等于
    TK_GT,        //>大于
    TK_GEQ,       //>=大于等于
    TK_ASSIGN,    //=赋值运算符
    TK_POINTSTO,  //->指向结构体成员运算符
    TK_DOT,       //.结构体成员运算符
    TK_AND,       //&地址与运算符
    TK_OPENPA,    //(左圆括号
    TK_CLOSEPA,   //)右圆括号
    TK_OPENBR,    //[左中括号
    TK_CLOSEBR,   //]右中括号
    TK_BEGIN,     //{左大括号
    TK_END,       //}右大括号
    TK_SEMICOLON, //;分号
    TK_COMMA,     //,逗号
    TK_ELLIPSIS,  //...省略号
    TK_EOF,       //文件结束符

    /*常量*/
    TK_CINT,  //整型常量
    TK_CCHAR, //字符常量
    TK_CSTR,  //字符串常量

    /*关键字*/
    KW_CHAR,     //char关键字
    KW_SHORT,    //short关键字
    KW_INT,      //int关键字
    KW_VOID,     //void关键字
    KW_STRUCT,   //struct关键字
    KW_IF,       //if关键字
    KW_ELSE,     //else关键字
    KW_FOR,      //for关键字
    KW_CONTINUE, //continue关键字
    KW_BREAK,    //break关键字
    KW_RETURN,   //return关键字
    KW_SIZEOF,   //sizeof关键字

    KW_ALIGN,   //__align关键字
    KW_CDECL,   //__cdecl关键字
    KW_STDCALL, //__stdcall关键字

    /*标识符*/
    TK_IDENT
};

/*词法状态枚举定义*/
enum e_LexState
{
    LEX_NORMAL,
    LEX_SEP
};

/*错误处理程序用到的枚举定义*/
/*错误级别*/
enum e_ErrorLevel
{
    LEVEL_WARNING,
    LEVEL_ERROR,
};

/*工作阶段*/
enum e_WorkStage
{
    STAGE_COMPILE,
    STAGE_LINK,
};

int elf_hash(char *key);
void *mallocz(int size);
TKWord *tkword_direct_insert(TKWord *tp);
TKWord *tkword_find(char *p, int keyno);
TKWord *tkword_insert(char *p);
void color_token(int lex_state);
void handle_exception(int stage, int level, char *fmt, va_list ap);
void warning(char *fmt, ...);
void error(char *fmt, ...);
void expect(char *msg);
char *get_tkstr(int v);
void skip(int c);
void link_error(char *fmt, ...);
void init();
void cleanup();
int calc_slign(int n, int align);

#endif // _UTILS_H