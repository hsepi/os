#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <stdlib.h>

int P(int semid, struct sembuf *buf) {
    //struct sembuf buf;
    buf->sem_op = -1;
    buf->sem_flg = 0;
    buf->sem_num = 0;

    return semop(semid, buf, 1);
}

int V(int semid, struct sembuf *buf) {
    //struct sembuf buf;
    buf->sem_op = 1;
    buf->sem_flg = 0;
    buf->sem_num = 0;

    return semop(semid, buf, 1);
}

void reverse(char* str, size_t sz) {
    for (int i = 0; i < sz / 2; i++) {
        char t = str[i];
        str[i] = str[sz - i - 1];
        str[sz - i - 1] = t;
    }
}

int main() {

    struct sembuf mybuf;

    int semid;

    int fd[2], result;
    size_t size;
    char resstring[14];

    /* Попытаемся создать pipe */
    if (pipe(fd) < 0) {
        /* Если создать pipe не удалось, печатаем об этом сообщение и прекращаем работу */
        printf("Can\'t create pipe\n");
        exit(-1);
    }

    key_t key; /* IPC ключ */
    char pathname[] = "08-4.c";
    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((semid = semget(key, 1, 0666)) < 0) {
        printf("Semaphore not found. Trying to create...\n");
        if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
            printf("Can\'t get semid\n");
            exit(-1);
        }
        printf("Create successful!\n");
    }

    result = fork();

    if (result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) {
        srand(time(NULL));
        for (int i = 0; i < rand() * 1000; ++i);
        size = write(fd[1], "Hello, world!", 14);

        if (size != 14) {
            printf("Can\'t write all string\n");
            exit(-1);
        } else {
            printf("Parent wrote his message.\n");
        }

        P(semid, &mybuf); // wait the child to write

        size = read(fd[0], resstring, 14);
        if (size < 0) {
            printf("Can\'t read string\n");
            exit(-1);
        }

        printf("Parent read: %s\n", resstring);

        close(fd[1]);
        close(fd[0]);
        printf("Parent exit\n");
    } else {
        size = read(fd[0], resstring, 14);
        if (size < 0) {
            printf("Can\'t read string\n");
            exit(-1);
        }
        printf("Child read: %s\n", resstring);

        reverse(resstring, 13);
        size = write(fd[1], resstring, 14);
        V(semid, &mybuf); // let the parent read

        if (size != 14) {
            printf("Can\'t write all string\n");
            exit(-1);
        } else {
            printf("Child wrote his message.\n");
        }

        close(fd[1]);
        close(fd[0]);
        printf("Child exit\n");
    }

    return 0;
}