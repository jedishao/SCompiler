#include "../head/utils.h"

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
        else if (token >= KW_CHAR) /*关键字为绿色*/
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
  * 功能:   计算字节对齐位置
  * n:      未对齐前值
  * align:  对齐粒度
 ********************************************/
int calc_align(int n, int align)
{
    return ((n+align-1)&(~(align-1)));
}