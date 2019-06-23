#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
int main()
{
    int fd;
    size_t size;

    (void)umask(0);

    if ((fd = open("myfile", O_RDONLY)) < 0) {
        printf("Can\'t open file\n");
        exit(-1);
    }

    char string[14];
    size = read(fd, string, 14);
    if (size != 14) {
        printf("Can\'t read all string\n");
        exit(-1);
    }

    printf("%s\n", string);

    if (close(fd) < 0)
        printf("Can\'t close file\n");
    return 0;
}
