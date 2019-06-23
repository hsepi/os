#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MSG_LEN 100

int main() {

    srand(time(NULL));

    int msqid;

    char pathname[] = "09-4server.c";

    key_t key;

    int len, maxlen;

    struct clientmsgbuf {
        long mtype;
        struct {
            pid_t pid;
            char message[MSG_LEN];
        } info;
    } clientbuf;

    struct servermsgbuf {
        long mtype;
        struct {
            char message[MSG_LEN];
        } info;
    } serverbuf;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msqid\n");
        exit(-1);
    }

    clientbuf.mtype = 1;
    clientbuf.info.pid = getpid();

    len = sizeof(clientbuf.info);
    sprintf(clientbuf.info.message, "%d", rand());

    printf("Client #%d sends message %s\n", clientbuf.info.pid, clientbuf.info.message);
    if (msgsnd(msqid, (struct clientmsgbuf *) &clientbuf, len, 0) < 0) {
        printf("Can't send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    printf("Waiting for response\n");

    maxlen = sizeof(serverbuf.info);
    if (len = msgrcv(msqid, &serverbuf, maxlen, getpid(), 0) < 0) {
        printf("Can't receive message from queue\n");
        exit(-1);
    }
    printf("Server: %s\n", serverbuf.info.message);

    return 0;

}





