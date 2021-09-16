#include<stdlib.h>
#include <stdio.h>
#include<malloc.h>

typedef struct Dyn
{
    int count;      //动态数组元素个数
    int capacity;   //动态数组缓冲区长度
    void **data;    //指向数据指针数组
}Dyn;


void dyn_init(Dyn *parr,int initsize)
{
    if(parr!=NULL)
    {
        parr->data=(void**)malloc(sizeof(char)*initsize);
        parr->count=0;
        parr->capacity=initsize;
    }
}
int main()
{
    // Dyn p;
    // Dyn *parr=&p;
    int **t;
    int **s;
    int a=122;
    int b=6;
    t=(int**)malloc(sizeof(char)*10);
  
    t[0]=&a;
    t[1]=&b;
    
    s=t;
    char **p;
    p=(char**)t;
    printf("t %d\n",t);
    printf("t[0] %d\n",t[0]);
    printf("*t %d\n",*t);
    printf("**t %d\n",**t);
    printf("%d\n",**p);
    printf("%d\n",sizeof(char)*10);
    printf("===================================================");
    int j[2]={1,2};
    printf("%d\n",j[1]);
    printf("%d\n",j[2]);
    return 0;
   
}

