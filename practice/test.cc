
#include<stdio.h>
#define LOG(fmt,...) printf("[%s:%d]" fmt "\n" ,__FILE__,__LINE__,##__VA_ARGS__)
int main()
{
    LOG("%s-%s","hello","world");
    LOG("%d,%d",1,2);
    return 0;
}

