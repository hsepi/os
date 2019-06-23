#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    char* directory_name = malloc(sizeof(char) * (PATH_MAX + 1));
    if (argc == 2) {
        directory_name = argv[1];
    } else
    {
        directory_name = getcwd(NULL, sizeof(char) * (PATH_MAX + 1));
        if (directory_name == NULL) {
            printf("Error\n");
            return(-1);
        }
    }

    printf("Looking at the dir: %s\n", directory_name);
    
    DIR *dir = opendir(directory_name);
    struct dirent *file;
    while((file = readdir(dir)) != NULL) {
        printf("%s\n", file->d_name);
    }

    if (closedir(dir) != 0) {
        printf("Error closing dir!\n");
        return(-1);
    }

    return 0;
}