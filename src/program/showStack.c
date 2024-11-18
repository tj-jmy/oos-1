#include <stdio.h>
#include <sys.h>

void main1()
{
    printf("getpid2: %d\n", getpid2());
    printf("getppid: %x\n", getppid());
}

int sum(int var1, int var2) // 显式声明参数类型
{
    int count;
    version = 2;
    count = var1 + var2;

    // printf("var1 address: %x\n", &var1);
    // printf("var2 address: %x\n", &var2);
    // printf("count address: %x\n", &count);

    return (count);
}
