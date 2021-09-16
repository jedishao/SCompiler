#include<stdio.h>
int main()
{
    // int m[]={1,2,3,4,5,6,7,8,9,0};
    // int (*p)[4]=(int(*)[4])m;
    // printf("%d",p[2][1]);
    // int a[]={9,3,18,11,17,22,4};
    // int size = sizeof(a) / sizeof(a[0]);
    // int re = dyn(a,size);
    // printf("%d",re);

    int a;
    int i=0;
    int b[10];
    scanf("%d", &a);
    while (a!=0)
    {
        /* code */
        b[i]=a%2;
        i++;
        a=a/2;
    }
    for (i; i>0; i--)
    {
        /* code */
        printf("%d", b[i-1]);
    }
    printf("\n");
}



int dyn(int a[], int size)
{
    int max = a[0];
    //ma为最大差值
    int ma = 0;
    int j = 1;
    //size 为数组长度
    for (int i=1; i<size; i++){
        int tmp = max - a[i];
        if(tmp>ma){
            ma = tmp;
        }
    }
    while(j<size){
        if(a[j]>max){
            max = a[j];
            for (int i=j+1; i<size; i++){
                int tmp = max - a[i];
                if(tmp>ma){
                    ma = tmp;   
                }
            }
        }
        j++;
    }
    return ma;
}