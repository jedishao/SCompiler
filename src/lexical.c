#include <stdio.h>
#include <stdlib.h>
#include "../head/lexical.h"

//词法分析器

/*******************************************
  * 计算哈希地址
 *  key:哈希关键字(为了与SC语言区分)
 *  MAXKEY:哈希表长度
 ********************************************/
int elf_hash(char *key)
{
    int h = 0, g;
    while (*key)
    {
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h % MAXKEY;
}

/*******************************************
  * 单词表操作函数
  * 功能:运算符、关键字、常量直接放入单词表
 ********************************************/
TKWord *tkword_dirext_insert(TKWord *tp)
{
    int keyno;
    dynarray_add(&tktable, tp);
    keyno = elf_hash(tp->spelling);
    tp->next = tk_hashtable[keyno];
    tk_hashtable[keyno] = tp;
    return tp;
}

/*******************************************
  * 功能:在单词表中查找单词
  * p:要查找的单词
  * keyno:要查找单词的哈希
 ********************************************/
TKWord *tkword_find(char *p, int keyno)
{
    TKWord *tp = NULL, *tpl;
    for (tpl = tk_hashtable[keyno]; tpl; tpl = tpl->next)
    {
        if (!strcmp(p, tpl->spelling))
        {
            keyno = tpl->tkcode;
            tp = tpl;
        }
    }
    return tp;
}

/*******************************************
  * 功能:标识符插入单词表，先查找，查找不到再插入
  * 单词表
 ********************************************/
TKWord *tkword_insert(char *p)
{
    TKWord *tp;
    int keyno;
    char *s;
    char *end;
    int length;

    keyno = elf_hash(p);
    tp = tkword_find(p, keyno);
    if (tp == NULL)
    {
        length = strlen(p);
        tp = (TKWord *)mallocz(sizeof(TKWord) + length + 1);
        tp->next = tk_hashtable[keyno];
        tk_hashtable[keyno] = tp;
        dynarray_add(&tktable, tp);
        tp->tkcode = tktable.count - 1;
        s = (char *)tp + sizeof(TKWord);
        tp->spelling = (char *)s;
        for (end = p + length; p < end;)
        {
            *s++ = *p++;
        }
        *s = (char)'\0';
    }
    return tp;
}

/*******************************************
  * 功能:分配堆内存并将数据初始化为'0'
  * size:分配内存大小
 ********************************************/
void *mallocz(int size)
{
    void *ptr;
    ptr = malloc(size);
    if (!ptr && size)
        error("Memory allocation failed");
    memset(ptr, 0, size);
    return ptr;
}

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

        {TK_CHAR, NULL, "char", NULL, NULL},
        {TK_SHORT, NULL, "short", NULL, NULL},
        {TK_INT, NULL, "int", NULL, NULL},
        {TK_VOID, NULL, "void", NULL, NULL},
        {TK_STRUCT, NULL, "struct", NULL, NULL},

        {TK_IF, NULL, "if", NULL, NULL},
        {TK_ELSE, NULL, "else", NULL, NULL},
        {TK_FOR, NULL, "for", NULL, NULL},
        {TK_CONTINUE, NULL, "continue", NULL, NULL},
        {TK_BREAK, NULL, "break", NULL, NULL},
        {TK_RETURN, NULL, "return", NULL, NULL},
        {TK_SIZEOF, NULL, "sizeof", NULL, NULL},
        {TK_ALIGN, NULL, "__align", NULL, NULL},
        {TK_CDECL, NULL, "__cdecl", NULL, NULL},
        {TK_STDCALL, NULL, "__stdcall", NULL, NULL},
        {0, NULL, NULL, NULL, NULL},
    };

    dynarray_init(&tktable, 8);
    for (tp = &keywords[0]; tp->spelling != NULL; tp++)
        tkword_dirext_insert(tp);
}

/*******************************************
  * 异常处理
  * stage:编译阶段还是链接阶段
  * level:错误级别
  * fmt:参数输出格式
  * ap:可变参数列表
 ********************************************/
void handle_exception(int stage, int level, char *fmt, va_list ap)
{
    char buf[1024];
    vsprintf(buf, fmt, ap);
    if (stage == STAGE_COMPILE)
    {
        if (level == LEVEL_WARNING)
            printf("%s(line %d):Compile Warning:%s!\n", filename, line_num, buf);
        else
        {
            printf("%s(line %d):Compile Error:%s!\n", filename, line_num, buf);
            exit(1);
        }
    }
    else
    {
        printf("Link error:%s!\n", buf);
        exit(1);
    }
}

/*******************************************
  * 编译致命错误处理
  * fmt:参数输出格式
  * ap:可变参数列表
 ********************************************/
void warning(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/*******************************************
  * 提示错误，此处缺少某个语法成分
  * msg:需要什么语法成分
 ********************************************/
void expect(char *msg)
{
    error("miss%s", msg);
}

/*******************************************
  * 链接错误处理
  * fmt:参数输出格式
  * ap:可变参数列表
 ********************************************/
void link_error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_LINK, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/*******************************************
  * 编译致命错误处理
  * fmt:参数输出格式
  * ap:可变参数列表
 ********************************************/
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/*******************************************
  * 跳过单词c，取下一个单词，如果当前单词不是c，
  * 提示错误
  * c:要跳过的单词
 ********************************************/
void skip(int c)
{
    if (token != c)
        error("miss'%s'", get_tkstr(c));
    get_token();
}

/*******************************************
  * 功能:取得单词v所代表的源码字符串
  * v:单词编号
 ********************************************/
char *get_tkstr(int v)
{
    if (v > tktable.count)
        return NULL;
    else if (v >= TK_CINT && v <= TK_CSTR)
        return sourcestr.data;
    else
        return ((TKWord *)tktable.data[v])->spelling;
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
  * 功能:词法着色
 ********************************************/
void color_token(int lex_state)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    char *p;
    switch (lex_state)
    {
    case LEX_NORMAL:
    {
        if (token >= TK_IDENT) /*标识符灰色*/
            SetConsoleTextAttribute(h, FOREGROUND_INTENSITY);
        else if (token >= TK_CHAR) /*关键字为绿色*/
            SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        else if (token >= TK_CINT) /*常量为黄色*/
            SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_RED);
        else /*运算符及分隔符为红色*/
            SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        p = get_tkstr(token);
        printf("%s", p);
        break;
    }
    case LEX_SEP:
        printf("%c", ch);
        break;
    }
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