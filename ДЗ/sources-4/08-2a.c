/* Программа 1 для иллюстрации работы с семафорами */

/* Эта программа получает доступ к одному системному семафору,
ждет пока его значение не станет больше или равным 1 после запусков программы 2,
а затем уменьшает его на 1*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

int main()
{

    int semid;

    char pathname[] = "08-2a.c";

    key_t key;

    struct sembuf mybuf;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get semid\n");
        exit(-1);
    }

    mybuf.sem_op = -5;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop(semid, &mybuf, 1) < 0) {
        printf("Can\'t wait for condition\n");
        exit(-1);
    }

    printf("Condition is present\n");
    return 0;
}