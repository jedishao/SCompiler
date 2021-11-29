#include "../head/utils.h"

/*******************************************
  * �����ϣ��ַ
 *  key:��ϣ�ؼ���(Ϊ����SC��������)
 *  MAXKEY:��ϣ����
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
  * ���ʱ��������
  * ����:��������ؼ��֡�����ֱ�ӷ��뵥�ʱ�
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
  * ����:�ڵ��ʱ��в��ҵ���
  * p:Ҫ���ҵĵ���
  * keyno:Ҫ���ҵ��ʵĹ�ϣ
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
  * ����:��ʶ�����뵥�ʱ��Ȳ��ң����Ҳ����ٲ���
  * ���ʱ�
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
  * ����:������ڴ沢�����ݳ�ʼ��Ϊ'0'
  * size:�����ڴ��С
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
  * �쳣����
  * stage:����׶λ������ӽ׶�
  * level:���󼶱�
  * fmt:���������ʽ
  * ap:�ɱ�����б�
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
  * ��������������
  * fmt:���������ʽ
  * ap:�ɱ�����б�
 ********************************************/
void warning(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/*******************************************
  * ��ʾ���󣬴˴�ȱ��ĳ���﷨�ɷ�
  * msg:��Ҫʲô�﷨�ɷ�
 ********************************************/
void expect(char *msg)
{
    error("miss%s", msg);
}

/*******************************************
  * ���Ӵ�����
  * fmt:���������ʽ
  * ap:�ɱ�����б�
 ********************************************/
void link_error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_LINK, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/*******************************************
  * ��������������
  * fmt:���������ʽ
  * ap:�ɱ�����б�
 ********************************************/
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/*******************************************
  * ��������c��ȡ��һ�����ʣ������ǰ���ʲ���c��
  * ��ʾ����
  * c:Ҫ�����ĵ���
 ********************************************/
void skip(int c)
{
    if (token != c)
        error("miss'%s'", get_tkstr(c));
    get_token();
}

/*******************************************
  * ����:ȡ�õ���v�������Դ���ַ���
  * v:���ʱ��
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
  * ����:�ʷ���ɫ
 ********************************************/
void color_token(int lex_state)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    char *p;
    switch (lex_state)
    {
    case LEX_NORMAL:
    {
        if (token >= TK_IDENT) /*��ʶ����ɫ*/
            SetConsoleTextAttribute(h, FOREGROUND_INTENSITY);
        else if (token >= KW_CHAR) /*�ؼ���Ϊ��ɫ*/
            SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        else if (token >= TK_CINT) /*����Ϊ��ɫ*/
            SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_RED);
        else /*��������ָ���Ϊ��ɫ*/
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
  * ����:   �����ֽڶ���λ��
  * n:      δ����ǰֵ
  * align:  ��������
 ********************************************/
int calc_align(int n, int align)
{
    return ((n+align-1)&(~(align-1)));
}