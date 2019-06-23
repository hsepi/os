#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>

int main() {
    int msqid;

    char pathname[] = "09-3a.c";

    key_t key;

    int len, maxlen;

    struct mymsgbuf {
        long mtype;
        struct {
            short sinfo;
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

    printf("2nd program started receiving messages from 1st.\n");
    for (int i = 0; i < 5; ++i) {
        maxlen = sizeof(mybuf.info);
        if (len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 1, 0) < 0) {
            printf("Can't receive message from queue\n");
            exit(-1);
        }

        printf("2nd program received message type = %ld, sInfo = %i\n", mybuf.mtype, mybuf.info.sinfo);
    }

    printf("2nd program finished receiving messages from 1st.\n");
    printf("2nd program started sending messages to first.\n");

    for (int i = 0; i < 5; ++i) {
        mybuf.mtype = 2;
        mybuf.info.sinfo = 7331;
        len = sizeof(mybuf.info);

        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
            printf("Can't send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
    }

    printf("2nd program finished sending messages to first.\n");

    return 0;
}





