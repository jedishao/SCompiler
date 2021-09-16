#include<stdio.h>
#include<stdlib.h>

int main()
{
    char str[80], max[80];
    int len, maxlen;
    printf("input some char:\n");
    gets(str);
    strcpy(max, str);
    while(str[0]!='\0')
    {
        gets(str);
        len=strlen(str);
        if(len>maxlen)
        {
            strcpy(max,str);
            maxlen=len;
        }
    }
    printf("string:%s, length:%d", max, maxlen);
    return 0;
}