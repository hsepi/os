#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Структура запроса клиента серверу
struct request_msg_buf {
    long mtype;
    struct {
        pid_t client_pid;
        long n;
    } info;
};

// Структура ответа сервера клиенту
struct response_msg_buf {
    long mtype;
};

int main() {

    srand(time(NULL));

    int msqid; // IPC-дескриптор очереди сообщений

    char pathname[] = "server.c"; // Файл, использующийся для генерации ключа

    key_t key; // IPC-ключ

    struct request_msg_buf request;
    struct response_msg_buf response;

    // Генерируем IPC-ключ
    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can't generate key\n");
        exit(-1);
    }
    // Получаем IPC-дескриптор очереди сообщений
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msqid\n");
        exit(-1);
    }

    // Сервер ждёт на вход сообщения с типом 1
    // (ни один пользовательский процесс не может иметь PID 1,
    // т.к. такой PID закреплён за инициализирующим системным процессом)
    request.mtype = 1;

    // Получение PID текущего процесса (для того, чтобы он мог ждать
    // ответного сообщения с типом, равным своему PID)
    request.info.client_pid = getpid();

    // Псевдослучайное число от -5 до 5
    // Если число > 0, то ресурсы освобождаются процессом
    // Если число < 0, то ресурсы запрашиваются процессом
    request.info.n = (rand() % 11) - 5;

    // Отправляем запрос серверу
    if (msgsnd(msqid, (struct msgbuf*) &request, sizeof(request.info), 0) < 0) {
        printf("Can't send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*) NULL);
        exit(-1);
    }

    if (request.info.n >= 0)
        printf("Freeing up %ld resources\n", request.info.n);
    else
        printf("Waiting for %ld resources\n", -request.info.n);
    msgrcv(msqid, (struct msgbuf*) &response, 0, request.info.client_pid, 0);

    printf("All right, process is terminated.\n");

    return 0;
}