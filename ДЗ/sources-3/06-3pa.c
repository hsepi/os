#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
int *want;
int *waiting;

void lock()
{
    want[0] = 1;
    *waiting = 0;
    while (want[1] == 1 && *waiting == 0);
}

void unlock()
{
    want[0] = 0;
}

int main()
{
    int *array;
    int shmid;
    int new = 1;
    char pathname[] = "06-3pa.c";
    key_t key;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((shmid = shmget(key, 6 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        if (errno != EEXIST) {
            printf("Can\'t create shared memory\n");
            exit(-1);
        } else {
            if ((shmid = shmget(key, 3 * sizeof(int), 0)) < 0) {
                printf("Can\'t find shared memory\n");
                exit(-1);
            }
            new = 0;
        }
    }
    if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }
    want = array + 3;
    waiting = array + 5;
    if (new) {
        array[0] = 1;
        array[1] = 0;
        array[2] = 1;
    } else {
        lock();
        array[0] += 1;
        for (long i = 0; i < 1000000000L; i++);
        array[2] += 1;
        unlock();
    }
    printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n", array[0], array[1], array[2]);

    if (shmdt(array) < 0) {
        printf("Can't detach shared memory\n");
        exit(-1);
    }
    return 0;
}
