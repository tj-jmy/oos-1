#include <stdio.h>
#include <sys.h>

void main1()
{
    int i, j;
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
            exit(2);
        }
    }
    else
    {
        printf("1st Child, pid = %d\n", getpid());
        exit(1);
    }
    while ((i = wait(&j)) != -1)
    {
        printf("process %d exit, exit code = %d\n", i, j);
    }

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
            exit(4);
        }
    }
    else
    {
        printf("3rd Child, pid = %d\n", getpid());
        exit(3);
    }
    while ((i = wait(&j)) != -1)
    {
        printf("process %d exit, exit code = %d\n", i, j);
    }
    printf("Parent end\n");
    exit(0);
}
