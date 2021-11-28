#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <Windows.h>
#include "dynarray.h"
#include "dynstring.h"

/*���ʴ洢�ṹ����*/
typedef struct TKWord
{
    int tkcode;                    //���ʱ���
    struct TKWord *next;           //ָ���ϣ��ͻ��ͬ���
    char *spelling;                //�����ַ���
    struct Symbol *sym_struct;     //ָ�򵥴�����ʾ�Ľṹ����
    struct Symbol *sym_identifier; //ָ�򵥴�����ʾ�ı�ʶ��
} TKWord;

#define MAXKEY 1024           //��ϣ������
TKWord *tk_hashtable[MAXKEY]; //���ʹ�ϣ��
DynArray tktable;             //���ʱ�

int token;
char ch;
FILE *fin;

char *filename;
int line_num;

DynString tkstr, sourcestr; //�����ַ���, ����Դ���ַ���
int tkvalue;

/*���ʱ���*/
enum e_Token_Code
{
    /*��������ָ���*/
    TK_PLUS,      //+�Ӻ�
    TK_MINUS,     //-����
    TK_STAR,      //*�Ǻ�
    TK_DIVIDE,    ///����
    TK_MOD,       //%���������
    TK_EQ,        //==���ں�
    TK_NEQ,       //��=������
    TK_LT,        //<С�ں�
    TK_LEQ,       //<=С�ڵ���
    TK_GT,        //>����
    TK_GEQ,       //>=���ڵ���
    TK_ASSIGN,    //=��ֵ�����
    TK_POINTSTO,  //->ָ��ṹ���Ա�����
    TK_DOT,       //.�ṹ���Ա�����
    TK_AND,       //&��ַ�������
    TK_OPENPA,    //(��Բ����
    TK_CLOSEPA,   //)��Բ����
    TK_OPENBR,    //[��������
    TK_CLOSEBR,   //]��������
    TK_BEGIN,     //{�������
    TK_END,       //}�Ҵ�����
    TK_SEMICOLON, //;�ֺ�
    TK_COMMA,     //,����
    TK_ELLIPSIS,  //...ʡ�Ժ�
    TK_EOF,       //�ļ�������

    /*����*/
    TK_CINT,  //���ͳ���
    TK_CCHAR, //�ַ�����
    TK_CSTR,  //�ַ�������

    /*�ؼ���*/
    KW_CHAR,     //char�ؼ���
    KW_SHORT,    //short�ؼ���
    KW_INT,      //int�ؼ���
    KW_VOID,     //void�ؼ���
    KW_STRUCT,   //struct�ؼ���
    KW_IF,       //if�ؼ���
    KW_ELSE,     //else�ؼ���
    KW_FOR,      //for�ؼ���
    KW_CONTINUE, //continue�ؼ���
    KW_BREAK,    //break�ؼ���
    KW_RETURN,   //return�ؼ���
    KW_SIZEOF,   //sizeof�ؼ���

    KW_ALIGN,   //__align�ؼ���
    KW_CDECL,   //__cdecl�ؼ���
    KW_STDCALL, //__stdcall�ؼ���

    /*��ʶ��*/
    TK_IDENT
};

/*�ʷ�״̬ö�ٶ���*/
enum e_LexState
{
    LEX_NORMAL,
    LEX_SEP
};

/*����������õ���ö�ٶ���*/
/*���󼶱�*/
enum e_ErrorLevel
{
    LEVEL_WARNING,
    LEVEL_ERROR,
};

/*�����׶�*/
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