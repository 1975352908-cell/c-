// 这是学习不定参宏函数的用例;
#include<stdio.h>
#include<iostream>
#include<stdarg.h>
#define LOG(fmt,...) printf("[%s:%d]" fmt "\n",__FILE__,__LINE__, ##__VA_ARGS__)
void printnum(int count,...)
{
    va_list ap;
    va_start(ap,count);
    for(int i=0;i<count;i++)
    {
        int num=va_arg(ap,int);
        printf("param[%d]:%d\n",i,num);
    }
    va_end(ap);
}
int main()
{
    LOG("%s-%s","王嘉钪是大帅哥","没错没错");
    return 0;
}