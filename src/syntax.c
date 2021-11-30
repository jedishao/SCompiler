#include <stdio.h>
#include <stdlib.h>
#include "..\head\syntax.h"

/*语法分析*/

/*
syntax_state    用于决定是否换行
syntax_level    控制缩进级别   
*/

/*******************************************
 * <translation_unit>::={<external_declaration>}<TK_EOF>
 * <翻译单元>::={<外部声明>}<文件结束符>
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
 * <外部声明>::=<函数定义>|<声明>
 * <函数定义>::=<类型区分符><声明符><函数体>
 * <声明>::=<类型区分符><分号>|<类型区分符><初值声明符表><分号>
 * <初值声明符表>::=<初值声明符>{<逗号><初值声明符>}
 * <初值声明符>::=<声明符>{<赋值运算符><初值符>}
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
    Type btype, type;
    int v, has_init, r, addr;
    Symbol *sym;
    if (!type_specifier(&btype))
    {
        expect("<type>");
    }

    if (btype.t == T_STRUCT && token == TK_SEMICOLON)
    {
        get_token();
        return;
    }

    while (1) //逐个分析声明或函数定义
    {
        type = btype;
        declarator(&type, &v, NULL);
        if (token == TK_BEGIN)
        {
            if (l == SC_LOCAL)
                error("no nesting call");
            if ((type.t & T_BTYPE) != T_FUNC)
                expect("<function definition>");

            sym = sym_search(v);
            if (sym) //函数签名声明过, 现在给出函数定义
            {
                if ((sym->type.t & T_BTYPE) != T_FUNC)
                    error("'%s'redefine", get_tkstr(v));
                sym->type = type;
            }
            else
            {
                sym = func_sym_push(v, &type);
            }
            sym->r = SC_SYM | SC_GLOBAL;
            funcbody(sym);
            break;
        }
        else //变量声明
        {
            r = 0;
            if (!(type.t & T_ARRAY))
                r |= SC_LVAL;
            r |= 1;
            has_init = (token == TK_ASSIGN);

            if (has_init)
            {
                get_token();
                initializer(&type);
            }
            sym = var_sym_push(&type, r, v, addr);
            // if (token == TK_ASSIGN)
            // {
            //     get_token();
            //     initializer();
            // }

            if (token == TK_COMMA)
            {
                get_token();
            }
            else
            {
                //syntax_state = SNTX_LF_HT;
                skip(TK_SEMICOLON);
                break;
            }
        }
    }
    // if(token==TK_SEMICOLON)
    //     skip(TK_SEMICOLON);
}

/*******************************************
 *  功能:         解析类型区分符
 *  type(输出):   数据类型
 *  返回值:       是否发现合法的类型区分符
 * 
 * <type_specifier>::=<KW_INT>
 *      |<TK_CHAR>
 *      |<TK_SHORT>
 *      |<TK_VOID>
 *      |<struct_specifier>
 ********************************************/
int type_specifier(Type *type)
{
    int t, type_found;
    Type type1;
    t = 0;
    type_found = 0;
    switch (token)
    {
    case KW_CHAR:
        t = T_CHAR;
        type_found = 1;
        //syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_SHORT:
        t = T_SHORT;
        type_found = 1;
        //syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_VOID:
        t = T_VOID;
        type_found = 1;
        //syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_INT:
        t = T_INT;
        type_found = 1;
        //syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_STRUCT:
        struct_specifier(&type1);
        type->ref = type1.ref;
        t = T_STRUCT;
        type_found = 1;
        // syntax_state = SNTX_SP;
        // struct_specifier();
        //get_token();
        break;
    default:
        break;
    }
    type->t = t;
    return type_found;
}

/*******************************************
 * 结构区分符
 * 
 * <struct_specifier>::=
 *      <TK_STRUCT><IDENTIFIER><TK_BEGIN>
 *      <struct_declaration_list><TK_END>|
 *      <TK_STRUCT><IDENTIFIER>
 * <结构区分符>::=<struct关键字><标识符>|
 *      <struct关键字><标识符><左大括号><结构声明表><右大括号>
 ********************************************/
void struct_specifier(Type *type)
{
    int v;
    Symbol *s;
    Type type1;

    get_token();
    v = token;

    syntax_state = SNTX_DELAY; //延迟到取出下一个单词后确定输出格式
    get_token();

    // if (token == TK_BEGIN) //适用于结构体定义
    //     syntax_state = SNTX_LF_HT;
    // else if (token == TK_CLOSEPA) //适用于sizeof(struct struct_name)
    //     syntax_state = SNTX_NUL;
    // else //适用于结构变量声明
    //     syntax_state = SNTX_SP;
    // syntax_indent();

    if (v < TK_IDENT) //关键字不能作为结构名称
        expect("struct");
    s = struct_search(v);
    if (!s)
    {
        type1.t = KW_STRUCT;
        //-1赋值给s->c, 标识结构体尚未定义
        s = sym_push(v | SC_STRUCT, &type1, 0, -1);
        s->r = 0;
    }

    type->t = T_STRUCT;
    type->ref = s;

    if (token == TK_BEGIN)
    {
        struct_declaration_list(type);
    }
}

/*******************************************
 * 结构声明表
 * 
 * <struct_declaration_list>::=
 *      <struct_declaration>{<struct_declaration>}
 * <结构声明表>::=
 *      <结构声明>{<结构声明>}
 ********************************************/
void struct_declaration_list(Type *type)
{
    int maxalign, offset;
    Symbol *s, **ps;

    // syntax_state = SNTX_LF_HT; //第一个结构体成员与'{'不写在一行
    // syntax_level++;            //结构体成员变量声明，缩进增加一级

    get_token();
    if (s->c != -1)
        error("struct already defined");
    maxalign = 1;
    ps = &s->next;
    offset = 0;
    while (token != TK_END)
    {
        //struct_declaration();
        struct_declaration(&maxalign, &offset, &ps);
    }
    skip(TK_END);

    //syntax_state = SNTX_LF_HT;
    s->c = calc_align(offset, maxalign); //结构体大小
    s->r = maxalign;                     //结构体对齐
}

/*******************************************
 * 功能:                   解析结构体成员声明
 * maxalign(输入, 输出):   成员最大对齐粒度
 * offset(输入, 输出):     偏移量
 * ps(输出):               结构定义符号
 * 
 * <struct_declaration>::=
 *      <type_specifier><struct_declarator_list><TK_SEMICOLON>*
 * <struct_declarator_list>::=<declarator>{<TK_COMMA><declarator>}
 * 
 * 等价转换后:
 * <struct_declaration>::=
 *      <type_specifier><declarator>{<TK_COMMA><declarator>}
 *      <TK_SEMICOLON>
 * <结构声明>::=<类型区分符><声明符>{<逗号><声明符>}<分号>
 ********************************************/
void struct_declaration(int *maxalign, int *offset, Symbol ***ps)
{
    int v, size, align;
    Symbol *ss;
    Type type1, btype;
    int force_align;
    type_specifier(&btype);

    while (1)
    {
        v = 0;
        type1 = btype;
        declarator(&type1, &v, &force_align);
        size = type_size(&type1, &align);

        if (force_align & ALIGN_SET)
            align = force_align & ~ALIGN_SET;
        *offset = calc_align(*offset, align);

        if (align > *maxalign)
            *maxalign = align;
        ss = sym_push(v | SC_MEMBER, &type1, 0, *offset);
        *offset += size;
        **ps = ss;
        *ps = &ss->next;

        if (token == TK_SEMICOLON)
            break;
        skip(TK_COMMA);
    }
    //syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 * 功能:                解析结构体成员对齐
 * force_align(输出):   强制对齐粒度
 * 
 * <struct_member_alignment>::=<TK_ALIGN><TK_OPENPA>
 *            <TK_CLINET><TK_CLOSEPA>
 * <结构成员对齐>::=<__align关键字><左小括号>
 *            <整数常量><右小括号>
 ********************************************/
void struct_member_alignment(int *force_align)
{
    int align = 1;
    if (token == KW_ALIGN)
    {
        get_token();
        skip(TK_OPENPA);
        if (token == TK_CINT)
        {
            get_token();
            align = tkvalue;
        }
        else
        {
            expect("integer constant");
        }
        skip(TK_CLOSEPA);
        if (align != 1 && align != 2 && align != 4)
            align = 1;
        align |= ALIGN_SET;
        *force_align = align;
    }
    else
    {
        *force_align = 1;
    }
}

/*******************************************
 * 功能:                解析函数调用约定
 * fc(输出):            调用约定
 * 
 *<function_calling_convention>::=<TK_CDECL>|<TK_STDCALL>
 *<调用约定>::=<__cdecl关键字>|<__stdcall关键字>
 *用于函数声明上，用在数据声明上忽略掉
 ********************************************/
void function_calling_convention(int *fc)
{
    *fc = KW_CDECL;
    if (token == KW_CDECL || token == KW_STDCALL)
    {
        *fc = token;
        //syntax_state = SNTX_SP;
        get_token();
    }
}

/*******************************************
 * 功能:                解析
 * type:                数据类型
 * v(输出):             单词编号
 * force_align(输出):   强制对齐粒度
 * 
 * <declarator>::={<pointer>}[<function_calling_convention>]
 *   [<struct_member_alignment>]<direct_declarator>
 * <pointer>::=<TK_STAR>
 * 
 * 等价转换后:
 * <declarator>::={<TK_STAR>}[<function_calling_convention>]
 *   [<struct_member_alignment>]<direct_declarator>
 ********************************************/
void declarator(Type *type, int *v, int *force_align)
{
    int fc;
    while (token == TK_STAR)
    {
        mk_pointer(type);
        get_token();
    }
    function_calling_convention(&fc);
    if (force_align)
        struct_member_alignment(force_align);
    direct_declarator(type, v, fc);
}

/*******************************************
 * 功能:              解析直接声明符
 * type(输入, 输出):  数据类型
 * v(输出):           单词编号
 * func_call:         函数调用约定
 * 
 * <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
*******************************************/
void direct_declarator(Type *type, int *v, int func_call)
{
    if (token >= TK_IDENT)
    {
        *v = token;
        get_token();
    }
    else
        expect("identifier");
    direct_declarator_postfix(type, func_call);
}

/*******************************************
 * 功能:              直接声明符后缀
 * type(输入,输出):   数据类型
 * func_call:         函数调用约定
 * 
 * <direct_declarator_postfix>::={<TK_OPENBR><TK_CINT><TK_CLOSEBR>
 *    |<TK_OPENBR><TK_CLOSEBR>
 *    |<TK_OPENPA><parameter_type_list><TK_CLOSEPA>
 *    |<TK_OPENPA><TK_CLOSEPA>}
*******************************************/
void direct_declarator_postfix(Type *type, int func_call)
{
    int n;
    Symbol *s;

    if (token == TK_OPENPA)
    {
        parameter_type_list(type, func_call);
    }
    else if (token == TK_OPENBR)
    {
        get_token();
        n = -1;
        if (token == TK_CINT)
        {
            get_token();
            n = tkvalue;
        }
        skip(TK_CLOSEBR);
        direct_declarator_postfix(type, func_call);
        s = sym_push(SC_ANOM, type, 0, n);
        type->t = T_ARRAY | T_PTR;
        type->ref = s;
    }
}

/*******************************************
 * 功能:              解析形参类型表
 * type(输入,输出):   数据类型
 * func_call:         函数调用约定
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
void parameter_type_list(Type *type, int func_call)
{
    int n;
    Symbol **plast, *s, *first;
    Type pt;

    get_token();
    first = NULL;
    plast = &first;

    while (token != TK_CLOSEPA)
    {
        if (!type_specifier(&pt))
        {
            error("invaild spec");
        }
        declarator(&pt, &n, NULL);
        s = sym_push(n | SC_PAPAMS, &pt, 0, 0);
        *plast = s;
        plast = &s->next;
        if (token == TK_CLOSEPA)
            break;
        skip(TK_COMMA);
        if (token == TK_ELLIPSIS)
        {
            func_call = KW_CDECL;
            get_token();
            break;
        }
    }
    //syntax_state = SNTX_DELAY;
    skip(TK_CLOSEPA);
    //  if (token == TK_BEGIN) //函数定义
    //     syntax_state = SNTX_LF_HT;
    // else //函数声明
    //     syntax_state = SNTX_NUL;
    // syntax_indent();

    //此处将函数返回类型存储, 然后指向参数, 最后将type设为函数类型
    //引用的相关信息放在ref中
    s = sym_push(SC_ANOM, type, func_call, 0);
    s->next = first;
    type->t = T_FUNC;
    type->ref = s;
}

/*******************************************
 * 功能:  解析函数体
 * sym:   函数符号
 * <funcbody>::=<compound_statement>
*******************************************/
void funcbody(Symbol *sym)
{
    //放一匿名符号在局部符号表中
    sym_direct_push(&local_sym_stack, SC_ANOM, &int_type, 0);
    compound_statement(NULL, NULL);
    //清空局部符号栈
    sym_pop(&local_sym_stack, NULL);
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
    case KW_IF:
        if_statement();
        break;
    case KW_RETURN:
        return_statement();
        break;
    case KW_BREAK:
        break_statement();
        break;
    case KW_CONTINUE:
        continue_statement();
        break;
    case KW_FOR:
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
    case KW_CHAR:
    case KW_SHORT:
    case KW_INT:
    case KW_VOID:
    case KW_STRUCT:
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
    if (token != TK_SEMICOLON)
    {
        expression();
    }
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 * 条件判断
 * 
 * <if_statement>::=<KW_IF><TK_OPENPA><expression>
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
    if (token == KW_ELSE)
    {
        syntax_state = SNTX_LF_HT;
        get_token();
        statement();
    }
}

/*******************************************
 * for循环
 * 
 * <for_statement>::=<KW_FOR><TK_OPENPA><expression_statement>
 *      <expression_statement><expression><TK_CLOSEPA><statement>
*******************************************/
void for_statement()
{
    get_token();
    skip(TK_OPENPA);
    if (token != TK_SEMICOLON)
    {
        expression();
    }
    skip(TK_SEMICOLON);
    if (token != TK_SEMICOLON)
    {
        expression();
    }
    skip(TK_SEMICOLON);
    if (token != TK_CLOSEPA)
    {
        expression();
    }
    syntax_state = SNTX_LF_HT;
    skip(TK_CLOSEPA);
    statement();
}

/*******************************************
 * continue 语义上存在问题
 * 
 * <continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
*******************************************/
void continue_statement()
{
    get_token();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 * break
 * 
 * <break_statement>::=<KW_BREAK><TK_SEMICOLON>
*******************************************/
void break_statement()
{
    get_token();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 * return
 * 
 * <return_statement>::=<KW_RETURN><TK_SEMICOLON>
 *      |<KW_RETURN><expression><TK_SEMICOLON>
*******************************************/
void return_statement()
{
    syntax_state = SNTX_DELAY;
    get_token();
    if (token == TK_SEMICOLON) //适用于return;
        syntax_state = SNTX_NUL;
    else
        syntax_state = SNTX_SP; //适用于return <expression>;
    syntax_indent();

    if (token != TK_SEMICOLON)
        expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/*******************************************
 * expression
 * 
 * <expression>::=<assignment_expression>
 *      {<TK_COMMA><assignment_expression>}
*******************************************/
void expression()
{
    while (1)
    {
        assignment_expression();
        if (token != TK_COMMA)
            break;
        get_token();
    }
}

/*******************************************
 * 赋值表达式
 * <assignment_expression>::=<equality_expression>
 *      |<unary_expression><TK_ASSIGN><assignment_expression>
 * 
 * 非等价变换后:(语义上存在问题)
 * <assignment_expression>::=<equality_expression>
 *      {<TK_ASSIGN>><assignment_expression>}
*******************************************/
void assignment_expression()
{
    equality_expression();
    if (token == TK_ASSIGN)
    {
        get_token();
        assignment_expression();
    }
}

/*******************************************
 * 相等类表达式
 * 
 * <equality_expression>::=<relational_expression>
 *      {<TK_EQ><relational_expression>
 *      |<TK_NEQ><relational_expression>}
*******************************************/
void equality_expression()
{
    relational_expression();
    while (token == TK_EQ || token == TK_NEQ)
    {
        get_token();
        relational_expression();
    }
}

/*******************************************
 * 关系类表达式
 * 
 * <relational_expression>::=<additive_expression>
 *      {<TK_LT><additive_expression>
 *      |<TK_GT><additive_expression>
 *      |<TK_LEQ><additive_expression>
 *      |<TK_GEQ><additive_expression>}
*******************************************/
void relational_expression()
{
    additive_expression();
    while ((token == TK_LT || token == TK_LEQ) ||
           (token == TK_GT || token == TK_GEQ))
    {
        get_token();
        additive_expression();
    }
}

/*******************************************
 * 加减类表达式
 * 
 * <additive_expression>::=<multiplication_expression>
 *      {<TK_PLUS><multiplication_expression>
 *      |<TK_MINUS><multiplication_expression>}
*******************************************/
void additive_expression()
{
    multiplication_expression();
    while (token == TK_PLUS || token == TK_MINUS)
    {
        get_token();
        multiplication_expression();
    }
}

/*******************************************
 * 乘除类表达式
 * 
 * <multiplication_expression>::=<unary_expression>
 *      {<TK_STAR><unary_expression>
 *      |<TK_DIVIDE><unary_expression>
 *      |<TK_MOD><unary_expression>}
*******************************************/
void multiplication_expression()
{
    unary_expression();
    while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD)
    {
        get_token();
        unary_expression();
    }
}

/*******************************************
 * 一元类表达式
 * 
 * <unary_expression>::=<postfix_expression>
 *      |<TK_AND><unary_expression>
 *      |<TK_STAR><unary_expression>
 *      |<TK_PLUS><unary_expression>
 *      |<TK_MINUS><unary_expression>
 *      |<sizeof_expression>
*******************************************/
void unary_expression()
{
    switch (token)
    {
    case TK_AND:
        get_token();
        unary_expression();
        break;
    case TK_STAR:
        get_token();
        unary_expression();
        break;
    case TK_PLUS:
        get_token();
        unary_expression();
        break;
    case TK_MINUS:
        get_token();
        unary_expression();
        break;
    case KW_SIZEOF:
        sizeof_expression();
        break;
    default:
        postfix_expression();
        break;
    }
}

/*******************************************
 * sizeof
 * 
 * <sizeof_expression>::=
 *      <KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
*******************************************/
void sizeof_expression()
{
    int align, size;
    Type type;

    get_token();
    skip(TK_OPENPA);
    type_specifier(&type);
    skip(TK_CLOSEPA);

    size = type_size(&type, &align);
    if (size < 0)
    {
        error("sizeof fail to compute size");
    }
}

/*******************************************
 * 后缀表达式
 * 
 * <postfix_expression>::=<primary_expression>
 *      {<TK_OPENBR><expression><TK_CLOSEBR>
 *      |<TK_OPENPA><TK_CLOSEPA>
 *      |<TK_OPENPA><argument_expression_list><TK_CLOSEPA>
 *      |<TK_DOT><IDENTIFIER>
 *      |<TK_POINTSTO><IDENTIFIER>}
*******************************************/
void postfix_expression()
{
    primary_expression();
    while (1)
    {
        if (token == TK_DOT || token == TK_POINTSTO)
        {
            get_token();
            //	token |= SC_MEMBER;  //This is to check whether it is a member of the struct
            get_token();
        }
        else if (token == TK_OPENBR)
        {
            get_token();
            expression();
            skip(TK_CLOSEBR);
        }
        else if (token == TK_OPENPA)
            argument_expression_list();
        else
            break;
    }
}

/*******************************************
 * 初值表达式
 * 
 * <primary_expression>::=<IDENTIFIER>
 *      |<TK_CINT>
 *      |<TK_CCHAR>
 *      |<TK_CSTR>
 *      |<TK_OPENPA><expression><TK_CLOSEPA>
*******************************************/
void primary_expression()
{
    int t;
    switch (token)
    {
    case TK_CINT:
    case TK_CCHAR:
    case TK_CSTR:
        get_token();
        break;
    case TK_OPENPA:
        get_token();
        expression();
        skip(TK_CLOSEPA);
        break;
    default:
        t = token;
        get_token();
        if (t < TK_IDENT)
            expect("identifier or constant");
        break;
    }
}

/*******************************************
 * 实参表达式表
 * 
 * <argument_expression_list>::=<argument_expression>
 *      {<TK_COMMA><assignment_expression>}
*******************************************/
void argument_expression_list()
{
    get_token();
    if (token != TK_CLOSEPA)
    {
        for (;;)
        {
            assignment_expression();
            if (token == TK_CLOSEPA)
                break;
            skip(TK_COMMA);
        }
    }
    skip(TK_CLOSEPA);
}

/*******************************************
 * 功能: 语法缩进
*******************************************/
void syntax_indent()
{
    switch (syntax_state)
    {
    case SNTX_NUL:
        color_token(LEX_NORMAL);
        break;
    case SNTX_SP:
        printf(" ");
        color_token(LEX_NORMAL);
        break;
    case SNTX_LF_HT:
        if (token == TK_END)
            syntax_level--;
        printf("\n");
        print_tab(syntax_level);
        color_token(LEX_NORMAL);
        break;
    case SNTX_DELAY:
        break;
    }
    syntax_state = SNTX_NUL;
}

/*******************************************
 * 功能: 缩进n个tab
 * n:    缩进个数
*******************************************/
void print_tab(int n)
{
    int i = 0;
    for (; i < n; i++)
        printf("\t");
}

int main(int argc, char **argv)
{
    fin = fopen("C:/C/SCompiler/src/syntest.c", "rb");
    if (!fin)
    {
        printf("can not open the file!\n");
        return 0;
    }

    init();
    getch();
    get_token();
    translation_unit();
    cleanup();
    fclose(fin);
    printf("\n%s success!\n", argv[1]);
    return 1;
}