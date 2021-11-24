#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../head/dynstack.h"

/*******************************************
  * ����:     ��ʼ��ջ�洢����
  * stack:    ջ�洢�ṹ
  * initsize: ջ��ʼ������ռ�
 ********************************************/
void stack_init(Stack *stack, int initsize)
{
    stack->base = (void **)malloc(sizeof(void **) * initsize);
    if (!stack->base)
    {
        error("fail to allocate memory");
    }
    else
    {
        stack->top = stack->base;
        stack->stacksize = initsize;
    }
}

/*******************************************
  * ����:     ����Ԫ��elementΪ�µ�ջ��Ԫ��
  * stack:    ջ�洢�ṹ
  * element:  Ҫ����ջ������Ԫ��
  * size:     ջԪ��ʵ�����ݳߴ�
  * ����ֵ:   ջ��Ԫ��
 ********************************************/
void *stack_push(Stack *stack, void *element, int size)
{
    int newsize;
    if (stack->top >= stack->base + stack->stacksize)
    {
        newsize = stack->stacksize * 2;
        stack->base = (void **)realloc(stack->stacksize, (sizeof(void **) * newsize));
        if (!stack->base)
        {
            return NULL;
        }
        stack->top = stack->base + stack->stacksize;
        stack->stacksize = newsize;
    }
    *stack->top = (void **)malloc(size);
    memcpy(*stack->top, element, size);
    stack->top++;
    return *(stack->top - 1);
}

/*******************************************
  * ����:     ����ջ��Ԫ��
  * stack:    ջ�洢�ṹ
 ********************************************/
void stack_pop(Stack *stack)
{
    if (stack->top > stack->base)
    {
        free(*(--stack->top));
    }
}

/*******************************************
  * ����:     �õ�ջ��Ԫ��
  * stack:    ջ�洢�ṹ
  * ����ֵ:   ջ��Ԫ��
 ********************************************/
void *stack_get_top(Stack *stack)
{
    void **element;
    if (stack->top > stack->base)
    {
        element = stack->top - 1;
        return *element;
    }
    else
    {
        return NULL;
    }
}

/*******************************************
  * ����:     �ж�ջ�Ƿ�Ϊ��
  * stack:    ջ�洢�ṹ
  * ����ֵ:   1��ʾջΪ��, 0��ʾջ�ǿ�
 ********************************************/
int stack_is_empty(Stack *stack)
{
    if (stack->top == stack->base)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************
  * ����:     ����ջ
  * stack:    ջ�洢�ṹ
 ********************************************/
void stack_destroy(Stack *stack)
{
    void **element;

    for (element = stack->base; element < stack->top; element++)
    {
        free(*element);
    }
    if (stack->base)
    {
        free(stack->base);
    }
    stack->base = NULL;
    stack->top = NULL;
    stack->stacksize = 0;
}