#include <stdio.h>
#include <stdarg.h>
void functestarg(int,...);
int main ()
{
functestarg(1,2,3,4,5,6,7,8,9,10,0);
return 0;
}
void functestarg(int a,...)
{
va_list argpointer;
va_start(argpointer, a);
int argument;
int count = 0;
while(0 != (argument = va_arg(argpointer, int)))
{
    printf("parameter%d:%d\n", ++count, argument);
}
}
