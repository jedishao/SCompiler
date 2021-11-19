#include <stdio.h>
#include <stdlib.h>
#include "..\head\utils.h"
#include "..\head\syntax.h"

/*语法分析*/

/*
syntax_state    用于决定是否换行
syntax_level    控制缩进级别   
*/

/*******************************************
 * <translation_unit>::={<external_declaration>}<TK_EOF>
 ********************************************/
void translation_unit()
{
    while (token != TK_EOF)
    {
        external_declaration(SC_GLOBAL);
    }
}

/*******************************************
 *  功能:解析外部声明
 *  l:存储类型，局部的还是全局的
 * <external_declaration>::=<function_definition>|<declaration>
 * <function_definition>::=<type_specifier><declarator><funcbody>
 * <declaration>::=<type_specifier><TK_SEMICOLON>
 *      |<type_specifier><init_declarator_list><TK_SEMICOLON>
 * <init_declarator_list>::=<init_declarator>{<TK_COMMA><init_declarator>}
 * <init_declarator>::=<declarator>{<TK_ASSIGN><initializer>}
 * 
 * 等价转换为LL(1)文法:
 * <external_declaration>::=
 *      <type_specifier>(<TK_SEMICOLON>
 *      |<declarator><funcbody>
 *      |<declarator>[<TK_ASSIGN><initializer>]
 *      {<TK_COMMA><declarator>)[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>)
 ********************************************/
void external_declaration(int l)
{
    if (!type_specifier())
    {
        expect("<类型区分符>");
    }

    if (token == TK_SEMICOLON)
    {
        get_token();
        return;
    }

    while (1) //逐个分析声明或函数定义
    {
        declarator();
        if (token == TK_BEGIN)
        {
            if (l == SC_LOCAL)
                error("不支持函数嵌套定义");
            funcbody();
            break;
        }
        else
        {
            if (token == TK_ASSIGN)
            {
                get_token();
                initializer();
            }

            if (token == TK_COMMA)
            {
                get_token();
            }
            else
            {
                syntax_state = SNTX_LF_HT;
                skip(TK_SEMICOLON);
                break;
            }
        }
    }
}

/*******************************************
 *  功能:解析类型区分符
 *  返回值:是否发现合法的类型区分符
 * 
 * <type_specifier>::=<KW_INT>
 *      |<TK_CHAR>
 *      |<TK_SHORT>
 *      |<TK_VOID>
 *      |<struct_specifier>
 ********************************************/
int type_specifier()
{
    int type_found = 0;
    switch (token)
    {
    case TK_CHAR:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case TK_SHORT:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case TK_VOID:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case TK_INT:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case TK_STRUCT:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    default:
        break;
    }
    return type_found;
}

/*******************************************
 * <struct_specifier>::=
 * <TK_STRUCT><IDENTIFIER><TK_BEGIN>
 * <struct_declaration_list><TK_END>|
 * <TK_STRUCT><IDENTIFIER>
 ********************************************/
void struct_specifier()
{
    int v;
    get_token();
    v = token;

    syntax_state = SNTX_DELAY; //延迟到取出下一个单词后确定输出格式
    get_token();

    if (token == TK_BEGIN) //适用于结构体定义
        syntax_state = SNTX_LF_HT;
    else if (token == TK_CLOSEPA) //适用于sizeod(struct struct_name)
        syntax_state = SNTX_NUL;
    else //适用于结构变量声明
        syntax_state = SNTX_SP;
    syntax_indent();

    if (v < TK_IDENT) //关键字不能作为结构名称
        expect("struct");

    if (token == TK_BEGIN)
    {
        struct_declaration_list();
    }
}

/*******************************************
 * <struct_declaration_list>::=
 *      <struct_declaration>{<struct_declaration>}
 ********************************************/
void struct_declaration_list()
{
    int maxalign, offset;

    syntax_state = SNTX_LF_HT; //第一个结构体成员与'{'不写在一行
    syntax_level++;            //结构体成员变量声明，缩进增加一级

    get_token();
    while (token != TK_END)
    {
        struct_declaration(&maxalign, &offset);
    }
    skip(TK_END);

    syntax_state = SNTX_LF_HT;
}

/*******************************************
 * <struct_declaration>::=
 *      <type_specifier><struct_declarator_list><TK_SEMICOLON>*
 * <struct_declarator_list>::=<declarator>{<TK_COMMA><declarator>}
 * 
 * 等价转换后:
 * <struct_declaration>::=
 *      <type_specifier><declarator>{<TK_COMMA><declarator>}
 *      <TK_SEMICOLON>
 ********************************************/
void struct_declaration()
{
    type_specifier();
    while (1)
    {
        declarator();

        if (token == TK_SEMICOLON)
            break;
        skip(TK_COMMA);
    }
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 *<function_calling_convention>::=<TK_CDECL>|<TK_STDCALL>
 *用于函数声明上，用在数据声明上忽略掉
 ********************************************/
void function_calling_convention(int *fc)
{
    *fc = TK_CDECL;
    if (token == TK_CDECL || token == TK_STDCALL)
    {
        *fc = token;
        syntax_state = SNTX_SP;
        get_token();
    }
}

/*******************************************
 * 结构成员对齐
 * <struct_member_alignment>::=<TK_ALIGN><TK_OPENPA>
 *            <TK_CLINET><TK_CLOSEPA>
 ********************************************/
void struct_member_alignment()
{
    if (token == TK_ALIGN)
    {
        get_token();
        skip(TK_OPENPA);
        if (token == TK_CINT)
        {
            get_token();
        }
        else
            expect("integer");
        skip(TK_CLOSEPA);
    }
}

/*******************************************
 * 声明符
 * <declarator>::={<pointer>}[<function_calling_convention>]
 *   [<struct_member_alignment>]<direcr_declarator>
 * <pointer>::=<TK_STAR>
 * 
 * 等价转换后:
 * <declarator>::={<TK_STAR>}[<function_calling_convention>]
 *   [<struct_member_alignment>]<direcr_declarator>
 ********************************************/
void declarator()
{
    int fc;
    while (token == TK_STAR)
    {
        get_token();
    }
    function_calling_convention(&fc);
    struct_member_alignment();
    direct_declarator();
}

/*******************************************
 * 直接声明符
 * <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
*******************************************/
void direct_declarator()
{
    if (token >= TK_IDENT)
    {
        get_token();
    }
    else
        expect("biaoshifu");
    direct_declarator_postfix();
}

/*******************************************
 * 直接声明符后缀
 * <direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>
 *    |<TK_OPENBR><TK_CLOSEBR>
 *    |<TK_OPENPA><parameter_type_list><TK_CLOSEPA>
 *    |<TK_OPENPA><TK_CLOSEPA>}
*******************************************/
void direcr_declarator_postfix()
{
    int n;
    if (token == TK_OPENPA)
    {
        parameter_type_list(n);
    }
    else if (token == TK_OPENBR)
    {
        get_token();
        if (token == TK_CINT)
        {
            get_token();
            n = tkvalue;
        }
        skip(TK_CLOSEBR);
        direcr_declarator_postfix();
    }
}

/*******************************************
 * 功能:解析形参类型表
 * func_call:函数调用约定
 * 
 * <parameter_type_list>::=<parameter_list>
 *    |<parameter_list><TK_COMMA><TK_ELLIPSIS>
 * <parameter_list>::=<paraameter_declaration>
 *    {<TK_COMMA><parameter_declaration>}
 * <parameter_declaration>::=<type_specifier>{<declarator>}
 * 
 * 等价转换后:
 * <parameter_type_list>::=<type_specifier>{<declarator>}
 *    {<TK_COMMA><type_specifier>{<declarator>}}<TK_COMMA><TK_ELLIPSIS>
 ********************************************/
void parameter_type_list(int func_call)
{
    get_token();
    while (token != TK_CLOSEPA)
    {
        if (!type_specifier())
        {
            error("invaild spec");
        }
        declarator();
        if (token == TK_CLOSEPA)
            break;
        skip(TK_COMMA);
        if (token == TK_ELLIPSIS)
        {
            func_call = TK_CDECL;
            get_token();
            break;
        }
    }
    syntax_state = SNTX_DELAY;
    skip(TK_CLOSEPA);
    if (token == TK_BEGIN) //函数定义
        syntax_state = SNTX_LF_HT;
    else //函数声明
        syntax_state = SNTX_NUL;
    syntax_indent();
}

/*******************************************
 * 函数体
 * <funcbody>::=<compound_statement>
*******************************************/
void funcbody()
{
    compound_statement();
}

/*******************************************
 * 初值符
 * <initializer>::=<assignment_expression>
*******************************************/
void initializer()
{
    assignment_expression();
}

/*******************************************
 * 语句
 * <statement>::=<compound_statement>
 *     |<if_statement>
 *     |<return_statement>
 *     |<break_statement>
 *     |<continue_statement>
 *     |<for_statement>
 *     |<expression_statement>
*******************************************/
void statement()
{
    switch (token)
    {
    case TK_BEGIN:
        compound_statement();
        break;
    case TK_IF:
        if_statement();
        break;
    case TK_RETURN:
        return_statement();
        break;
    case TK_BREAK:
        break_statement();
        break;
    case TK_CONTINUE:
        continue_statement();
        break;
    case TK_FOR:
        for_statement();
        break;
    default:
        expression_statement();
        break;
    }
}

/*******************************************
 * 复合语句
 * <compound_statement>::=<TK_BEGIN>{<delaration>}
 *           {<statement>}<TK_END>
*******************************************/
void compound_statement()
{
    syntax_state = SNTX_LF_HT;
    syntax_level++; //复合语句，缩进增加一级

    get_token();
    while (is_type_specifier(token))
    {
        external_declaration(SC_LOCAL);
    }
    while (token != TK_END)
    {
        statement();
    }

    syntax_state = SNTX_LF_HT;
    get_token();
}

/*******************************************
 * 功能:判断是否为类型区分符
 * v: 单词编码
*******************************************/
int is_type_specifier(int v)
{
    switch (v)
    {
    case TK_CHAR:
    case TK_SHORT:
    case TK_INT:
    case TK_VOID:
    case TK_STRUCT:
        return 1;
    default:
        break;
    }
    return 0;
}

/*******************************************
 * 表达式语句
 * <expression_statement>::=<TK_SEMICOLON>|
 *             <expression><TK_SEMICOLON>
*******************************************/
void expression_statement()
{
    if (token != token)
    {
        expression();
    }
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 * <if_statement>::=<TK_IF><TK_OPENPA><expression>
 *      <TK_CLOSEPA><statement>[<KW_ELSE><statement>]
*******************************************/
void if_statement()
{
    syntax_state = SNTX_SP;
    get_token();
    skip(TK_OPENPA);
    expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_CLOSEPA);
    statement();
    if (token == TK_ELSE)
    {
    }
}
