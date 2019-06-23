/**
 * 03-1.c program (modified, parent and child have different behavior)
 * from lectures "Основы операционных систем", 2005
 * by В.Е. Карпов, К.А. Коньков
 * page 327
 * 
 * Written by Zakharov Sergey aka Zakhse,
 * HSE, FCS SE, 2017
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();
    if (pid == -1)
        printf("Something went wrong, can't create child process\n");
    else if (pid == 0)
        printf("I'm child process with PID = %d\nMy parent's PID = %d\n\n", (int)getpid(), (int)getppid());
    else
    {
        printf("I'm parent with PID = %d now!\nMy child's PID = %d\n\n", (int)getpid(), (int)pid);
        sleep(1);
    }
    return 0;
}