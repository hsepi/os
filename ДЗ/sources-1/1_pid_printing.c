/**
 * Printing PID and PPID (parent's PID)
 * 
 * Written by Zakharov Sergey aka Zakhse,
 * HSE, FCS SE, 2017
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    printf("My pid = %d, my ppid = %d\n", (int)getpid(), (int)getppid());
    return 0;
}