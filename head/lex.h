#ifndef _LEX_H
#define _LEX_H

#include "utils.h"
void getch();
void get_token();
void preprocess();
void parse_comment();
void skip_white_space();
int is_nodigit(char c);
int is_digit(char c);
void parse_identifier();
void parse_num();
void parse_string(char sep);

#endif // !_LEX_H
