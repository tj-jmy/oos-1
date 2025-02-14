#include <stdio.h>
#include <sys.h>

void main1()
{
    int pid = getpid();
    printf("pid: %d\n", pid);
    unsigned int args[4] = {0, 0, 0, 0};
    getppda(args);
    printf("ppda_phy_addr: %x\n", args[0]);
    printf("ppda_vir_addr: %x\n", args[1]);
    printf("text_phy_addr: %x\n", args[2]);
    printf("text_vir_addr: %x\n", args[3]);
}
