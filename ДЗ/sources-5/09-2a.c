#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>

#define LAST_MESSAGE 255

int main() {
    int msqid;

    char pathname[] = "09-2a.c";

    key_t key;

    int len;

    struct mymsgbuf {
        long mtype;
        struct {
            short sinfo;
            float finfo;
        } info;
    } mybuf;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msqid\n");
        exit(-1);
    }

    for (int i = 0; i < 5; ++i) {
        mybuf.mtype = 1;
        mybuf.info.sinfo = 1337;
        mybuf.info.finfo = 13.37;
        len = sizeof(mybuf.info);

        if (msgsnd(msqid, &mybuf, len, 0) < 0) {
            printf("Can't send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
    }

    mybuf.mtype = LAST_MESSAGE;
    len = 0;
    if (msgsnd(msqid, &mybuf, len, 0) < 0) {
        printf("Can't send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    return 0;
}