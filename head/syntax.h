#ifndef _GRAMMER_H
#define _GRAMMER_H
#define ALIGN_SET 0x100
#include "lexical.h"
#include "utils.h"
#include "type.h"
#include "symbol.h"

int syntax_state; 	//语法状态
int syntax_level; 	//缩进级别

enum e_SynTaxState
{
	SNTX_NUL, 	//空状态, 没有语法缩进动作
	SNTX_SP,	//空格
	SNTX_LF_HT,	//换行并缩进, 每一个声明、函数定义、语句结束都要置为此状态
	SNTX_DELAY	//延迟到取出下一个单词后确定输出格式
};

enum e_Filed
{
	SC_GLOBAL,
	SC_LOCAL
};

void translation_unit();
void external_declaration(int l);
int type_specifier(Type *type);
void struct_specifier(Type *type);
void struct_declaration_list(Type *type);
void struct_declaration(int *maxalign, int *offset, Symbol ***ps);
void struct_member_alignment(int *force_align);
void function_calling_convention(int *fc);
void declarator(Type *type, int *v, int *force_align);
void direct_declarator(Type *type, int *v, int func_call);
void direct_declarator_postfix(Type *type, int func_call);
void parameter_type_list(Type *type, int func_call);
void funcbody(Symbol *sym);
void initializer();
void statement();
void compound_statement();
int is_type_specifier(int v);
void expression_statement();
void if_statement();
void for_statement();
void continue_statement();
void break_statement();
void return_statement();
void expression();
void assignment_expression();
void equality_expression();
void relational_expression();
void additive_expression();
void multiplication_expression();
void unary_expression();
void sizeof_expression();
void postfix_expression();
void primary_expression();
void argument_expression_list();
void syntax_indent();
void print_tab(int n);

#endif // !_YUFA_H