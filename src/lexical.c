#include <stdio.h>
#include <stdlib.h>
#include "../head/lexical.h"

//词法分析器

/*******************************************
  * 功能:词法分析初始化
 ********************************************/
void init_lex()
{
    TKWord *tp;
    static TKWord keywords[] = {
        {TK_PLUS, NULL, "+", NULL, NULL},
        {TK_MINUS, NULL, "-", NULL, NULL},
        {TK_STAR, NULL, "*", NULL, NULL},
        {TK_DIVIDE, NULL, "/", NULL, NULL},
        {TK_MOD, NULL, "&", NULL, NULL},
        {TK_EQ, NULL, "==", NULL, NULL},
        {TK_NEQ, NULL, "!=", NULL, NULL},
        {TK_LT, NULL, "<", NULL, NULL},
        {TK_LEQ, NULL, "<=", NULL, NULL},
        {TK_GT, NULL, ">", NULL, NULL},
        {TK_GEQ, NULL, ">=", NULL, NULL},
        {TK_ASSIGN, NULL, "=", NULL, NULL},
        {TK_POINTSTO, NULL, "->", NULL, NULL},
        {TK_DOT, NULL, ".", NULL, NULL},
        {TK_AND, NULL, "&", NULL, NULL},
        {TK_OPENPA, NULL, "(", NULL, NULL},
        {TK_CLOSEPA, NULL, ")", NULL, NULL},
        {TK_OPENBR, NULL, "[", NULL, NULL},
        {TK_CLOSEBR, NULL, "]", NULL, NULL},
        {TK_BEGIN, NULL, "{", NULL, NULL},
        {TK_END, NULL, "}", NULL, NULL},
        {TK_SEMICOLON, NULL, ";", NULL, NULL},
        {TK_COMMA, NULL, ",", NULL, NULL},
        {TK_ELLIPSIS, NULL, "...", NULL, NULL},
        {TK_EOF, NULL, "End_Of_File", NULL, NULL},

        {TK_CINT, NULL, "intconst", NULL, NULL},
        {TK_CCHAR, NULL, "charconst", NULL, NULL},
        {TK_CSTR, NULL, "strconst", NULL, NULL},

        {KW_CHAR, NULL, "char", NULL, NULL},
        {KW_SHORT, NULL, "short", NULL, NULL},
        {KW_INT, NULL, "int", NULL, NULL},
        {KW_VOID, NULL, "void", NULL, NULL},
        {KW_STRUCT, NULL, "struct", NULL, NULL},

        {KW_IF, NULL, "if", NULL, NULL},
        {KW_ELSE, NULL, "else", NULL, NULL},
        {KW_FOR, NULL, "for", NULL, NULL},
        {KW_CONTINUE, NULL, "continue", NULL, NULL},
        {KW_BREAK, NULL, "break", NULL, NULL},
        {KW_RETURN, NULL, "return", NULL, NULL},
        {KW_SIZEOF, NULL, "sizeof", NULL, NULL},
        {KW_ALIGN, NULL, "__align", NULL, NULL},
        {KW_CDECL, NULL, "__cdecl", NULL, NULL},
        {KW_STDCALL, NULL, "__stdcall", NULL, NULL},
        {0, NULL, NULL, NULL, NULL},
    };

    dynarray_init(&tktable, 8);
    for (tp = &keywords[0]; tp->spelling != NULL; tp++)
        tkword_dirext_insert(tp);
}

/*******************************************
  * 功能:取单词
 ********************************************/
void get_token()
{
    preprocess();
    switch (ch)
    {
    case 'a': case 'b': case 'c': case 'd':
    case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D':
    case 'E': case 'F': case 'G': case 'H':
    case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case '_':
    {
        TKWord *tp;
        parse_identifier();
        tp = tkword_insert(tkstr.data);
        token = tp->tkcode;
        break;
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
        parse_num();
        token = TK_CINT;
        break;
    }
    case '+':
    {
        getch();
        token = TK_PLUS;
        break;
    }
    case '-':
    {
        getch();
        if (ch == '>')
        {
            token = TK_POINTSTO;
            getch();
        }
        else
            token = TK_MINUS;
        break;
    }
    case '/':
    {
        token = TK_DIVIDE;
        getch();
        break;
    }
    case '%':
    {
        token = TK_MOD;
        getch();
        break;
    }
    case '=':
    {
        getch();
        if (ch == '=')
        {
            token = TK_EQ;
            getch();
        }
        else
            token = TK_ASSIGN;
        break;
    }
    case '!':
    {
        getch();
        if (ch == '=')
        {
            token = TK_NEQ;
            getch();
        }
        else
            error("not support gantan");
        break;
    }
    case '<':
    {
        getch();
        if (ch == '=')
        {
            token = TK_LEQ;
            getch();
        }
        else
            token = TK_LT;
        break;
    }
    case '>':
    {
        getch();
        if (ch == '=')
        {
            token = TK_GEQ;
            getch();
        }
        else
            token = TK_GT;
        break;
    }
    case '.':
    {
        getch();
        if (ch == '.')
        {
            getch();
            if (ch != '.')
                error("wrong with ellipsis");
            else
                token = TK_ELLIPSIS;
            getch();
        }
        else
            token = TK_DOT;
        break;
    }
    case '&':
    {
        token = TK_AND;
        getch();
        break;
    }
    case ';':
    {
        token = TK_SEMICOLON;
        getch();
        break;
    }
    case ']':
    {
        token = TK_CLOSEBR;
        getch();
        break;
    }
    case '}':
    {
        token = TK_END;
        getch();
        break;
    }
    case ')':
    {
        token = TK_CLOSEPA;
        getch();
        break;
    }
    case '[':
    {
        token = TK_OPENBR;
        getch();
        break;
    }
    case '{':
    {
        token = TK_BEGIN;
        getch();
        break;
    }
    case ',':
    {
        token = TK_COMMA;
        getch();
        break;
    }
    case '(':
    {
        token = TK_OPENPA;
        getch();
        break;
    }
    case '*':
    {
        token = TK_STAR;
        getch();
        break;
    }
    case '\'':
    {
        parse_string(ch);
        token = TK_CCHAR;
        tkvalue = *(char *)tkstr.data;
        break;
    }
    case '\"':
    {
        parse_string(ch);
        token = TK_CSTR;
        break;
    }
    case EOF:
    {
        token = TK_EOF;
        break;
    }
    default:
    {
        error("can not regonize:\\x%02x", ch);
        getch();
        break;
    }
    }
    syntax_indent();
}

/*******************************************
  * 功能:从sc源文件中读取一个字符
 ********************************************/
void getch()
{
    ch = fgetc(fin);
}

/*******************************************
  * 功能:预处理，忽略空白字符及注释
 ********************************************/
void preprocess()
{
    while (1)
    {
        if (ch == ' ' || ch == '\t' || ch == '\r')
            skip_white_space();
        else if (ch == '/')
        {
            //向前多读一个字节看是否是注释开始符，猜错了把多读的字符再放回去
            getch();
            if (ch == '*')
                parse_comment();
            else
            {
                ungetc(ch, fin); //把一个字符退回到输入流中
                ch = '/';
                break;
            }
        }
        else
            break;
    }
}

/*******************************************
  * 功能:解析注释
 ********************************************/
void parse_comment()
{
    getch();
    do
    {
        do
        {
            if (ch == '\n' || ch == '*' || ch == EOF)
                break;
            else
                getch();
        } while (1);
        if (ch == '\n')
        {
            line_num++;
            getch();
        }
        else if (ch == '*')
        {
            getch();
            if (ch == '/')
            {
                getch();
                return;
            }
        }
        else
        {
            error("wrong with end comment");
            return;
        }
    } while (1);
}

/*******************************************
  * 功能:忽略空格、Tab和回车
 ********************************************/
void skip_white_space()
{
    while (ch == ' ' || ch == '\t' || ch == '\r')
    {
        if (ch == '\r')
        {
            getch();
            if (ch != '\n')
                return;
            line_num++;
        }
        printf("%c", ch);
        getch();
    }
}

/*******************************************
  * 功能:判断c是否为字母或下划线
  * c:字符值
 ********************************************/
int is_nodigit(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

/*******************************************
  * 功能:判断c是否为数字
  * c:字符值
 ********************************************/
int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/*******************************************
  * 功能:解析标识符
 ********************************************/
void parse_identifier()
{
    dynstring_reset(&tkstr);
    dynstring_chcat(&tkstr, ch);
    getch();
    while (is_nodigit(ch) || is_digit(ch))
    {
        dynstring_chcat(&tkstr, ch);
        getch();
    }
    dynstring_chcat(&tkstr, '\0');
}

/*******************************************
  * 功能:解析整型常量
 ********************************************/
void parse_num()
{
    dynstring_reset(&tkstr);
    dynstring_reset(&sourcestr);
    do
    {
        dynstring_chcat(&tkstr, ch);
        dynstring_chcat(&sourcestr, ch);
        getch();
    } while (is_digit(ch));
    if (ch == '.')
    {
        do
        {
            dynstring_chcat(&tkstr, ch);
            dynstring_chcat(&sourcestr, ch);
            getch();
        } while (is_digit(ch));
    }
    dynstring_chcat(&tkstr, '\0');
    dynstring_chcat(&sourcestr, '\0');
    tkvalue = atoi(tkstr.data);
}

/*******************************************
  * 功能:解析字符常量、字符串常量
  * sep:字符常量界符标识为单引号(')
  *     字符串常量界符标识符为双引号(")
 ********************************************/
void parse_string(char sep)
{
    char c;
    dynstring_reset(&tkstr);
    dynstring_reset(&sourcestr);
    dynstring_chcat(&sourcestr, sep);
    getch();
    for (;;)
    {
        if (ch == sep)
            break;
        else if (ch == '\\')
        {
            dynstring_chcat(&sourcestr, ch);
            getch();
            switch (ch)
            {
            case '0':
                c = '\0';
                break;
            case 'a':
                c = '\a';
                break;
            case 'b':
                c = '\b';
                break;
            case 't':
                c = '\t';
                break;
            case 'n':
                c = '\n';
                break;
            case 'v':
                c = '\v';
                break;
            case 'f':
                c = '\f';
                break;
            case 'r':
                c = '\r';
                break;
            case '\"':
                c = '\"';
                break;
            case '\'':
                c = '\'';
                break;
            case '\\':
                c = '\\';
                break;
            default:
                c = ch;
                if (c >= '!' && c <= '~')
                    warning("非法转义字符:\'\\%c\'", c);
                else
                    warning("非法转义字符:\'\\0x%x\'", c);
                break;
            }
            dynstring_chcat(&tkstr, c);
            dynstring_chcat(&sourcestr, ch);
            getch();
        }
        else
        {
            dynstring_chcat(&tkstr, ch);
            dynstring_chcat(&sourcestr, ch);
            getch();
        }
    }
    dynstring_chcat(&tkstr, '\0');
    dynstring_chcat(&sourcestr, sep);
    dynstring_chcat(&sourcestr, '\0');
    getch();
}

/*******************************************
  * 功能:main主函数
 ********************************************/
int main(int argc, char **argv)
{
    fin = fopen(argv[1], "rb");
    if (!fin)
    {
        printf("can not open!\n");
        return 0;
    }
    init();

    getch();
    do
    {
        get_token();
        color_token(LEX_NORMAL);
    } while (token != TK_EOF);

    printf("\n line num:%d\n", line_num);

    cleanup();
    fclose(fin);
    printf("%ssucc", argv[1]);
    return 1;
}

/*******************************************
  * 功能:初始行
 ********************************************/
void init()
{
    line_num = 1;
    init_lex();
}

/*******************************************
  * 功能:扫尾工作
 ********************************************/
void cleanup()
{
    int i;
    printf("\n tktable.count=%d\n", tktable.count);
    for (i = TK_IDENT; i < tktable.count; i++)
    {
        free(tktable.data[i]);
    }
    free(tktable.data);
}