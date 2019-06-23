#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{

    (void) execle("/bin/cat", "/bin/cat", "03-2.c", 0, envp);

    printf("Error on program start\n");
    exit(-1);

    return 0;
}

