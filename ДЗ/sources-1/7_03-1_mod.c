/**
 * MODDED 03-1.c program
 * from lectures "Основы операционных систем", 2005
 * by В.Е. Карпов, К.А. Коньков
 * page 327
 * 
 * MODDED version (child runs another program with exec()
 * and switch() is used)
 * 
 * Written by Zakharov Sergey aka Zakhse,
 * HSE, FCS SE, 2017
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[])
{
    pid_t pid = fork();
    switch (pid) {
    case -1:
        printf("Something went wrong, can't create child process\n");
        break;
    case 0:
        execle("/bin/bash", "bash", "-c", "echo $PWD", 0, envp);
        break;
    default:
        break;
    }

    return 0;
}
