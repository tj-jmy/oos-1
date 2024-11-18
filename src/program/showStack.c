#include <stdio.h>
int version = 1;

void main1()
{
    int a, b, result;
    a = 1;
    b = 2;
    result = sum(a, b);

    // while (1)
    // {
    //     void *ptr = malloc(1024 * 1024); // 分配 1MB 内存
    //     printf("ptr=%x\n", ptr);
    //     if (ptr == 0)
    //     {
    //         printf("内存分配失败，可能已经耗尽所有可用内存。\n");
    //         break;
    //     }
    //     // 可以在这里做一些操作，但不要释放内存
    // }
    // return 0;

    printf("result=%d\n", result);

    // printf("main1 address: %x\n", main1);
    // printf("sum address: %x\n", sum);
    // printf("version address: %x\n", &version);
    // printf("a address: %x\n", &a);
    // printf("b address: %x\n", &b);
    // printf("result address: %x\n", &result);
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
