#include <stdio.h>
#include <sys.h>

void main1()
{
    int i;
    while ((i = fork()) == -1)
        ;
    if (i)
    {
        while ((i = fork()) == -1)
            ;
        if (i)
        {
            printf("Parent, pid = %d\n", getpid());
        }
        else
        {
            printf("2nd Child, pid = %d\n", getpid());
            exit(0);
        }
    }
    else
    {
        printf("1st Child, pid = %d\n", getpid());
        exit(0);
    }
    while (wait(0) != -1)
        ;
    while ((i = fork()) == -1)
        ;
    if (i)
    {
        while ((i = fork()) == -1)
            ;
        if (i)
        {
            printf("Parent, pid = %d\n", getpid());
        }
        else
        {
            printf("4th Child, pid = %d\n", getpid());
            exit(0);
        }
    }
    else
    {
        printf("3rd Child, pid = %d\n", getpid());
        exit(0);
    }
    while (wait(0) != -1)
        ;
    printf("Parent end\n");
    exit(0);
}
