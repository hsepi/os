/**
 * Printing command line arguments and environment parameters
 * 
 * Written by Zakharov Sergey aka Zakhse,
 * HSE, FCS SE, 2017
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[])
{
    printf("Command line arguments:\n");
    for (int i = 0; i < argc; ++i)
        printf("%s\n", argv[i]);
    printf("\n");

    printf("Environment parameters:\n");
    int i = 0;
    while (envp[i] != NULL)
        printf("%s\n", envp[i++]);
    printf("\n");

    return 0;
}