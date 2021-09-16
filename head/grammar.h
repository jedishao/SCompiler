#ifndef _GRAMMER_H
#define _GRAMMER_H

#include "lex.h"

int syntax_state;//�﷨״̬
int syntax_level;

enum e_SynTaxState
{
	SNTX_NUL,//��״̬
	SNTX_SP,
	SNTX_LF_HT,
	SNTX_DELAY
};

enum e_Filed
{
	SC_GLOBAL,
	SC_LOCAL
};

void translation_unit();
void external_declaration(int l);
int type_specifier();
void struct_specifier();
void struct_declaration_list();
void struct_declaration();
void function_calling_convention(int *fc);
void struct_member_alignment();
void declarator();
void direct_declarator();
void direct_declarator_postfix();
void parameter_type_list();
void funcbody();
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