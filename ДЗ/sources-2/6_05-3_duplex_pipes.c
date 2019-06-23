#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void reverse(char* str, size_t sz);

int main()
{
    int pipe_parent[2], pipe_child[2], result;
    size_t size;
    char resstring[14];

    if (pipe(pipe_parent) < 0) {
        printf("Can\'t create parent pipe\n");
        exit(-1);
    }

    if (pipe(pipe_child) < 0) {
        printf("Can\'t create child pipe\n");
        exit(-1);
    }

    result = fork();
    if (result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) {
        // parent
        close(pipe_parent[0]);
        close(pipe_child[1]);
        size = write(pipe_parent[1], "Hello, world!", 14);
        if (size != 14) {
            printf("Can\'t write all string\n");
            exit(-1);
        }
        close(pipe_parent[1]);

        size = read(pipe_child[0], resstring, 14);
        if (size != 14) {
            printf("Can\'t read from child\n");
            exit(-1);
        }
        printf("Parent got from child: %s\n", resstring);
        printf("Parent exit\n");
    } else {
        close(pipe_parent[1]);
        close(pipe_child[0]);
        size = read(pipe_parent[0], resstring, 14);
        if (size < 0) {
            printf("Can\'t get from parent\n");
            exit(-1);
        }
        printf("Got from parent: %s\n", resstring);
        reverse(resstring, 13);
        size = write(pipe_child[1], resstring, 14);
        close(pipe_parent[0]);
        printf("Exit child\n");
    }
    return 0;
}

void reverse(char* str, size_t sz)
{
    for (int i = 0; i < sz / 2; i++) {
        char t = str[i];
        str[i] = str[sz - i - 1];
        str[sz - i - 1] = t;
    }
}
