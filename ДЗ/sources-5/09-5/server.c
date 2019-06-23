#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

// Узел очереди из sys/queue.h
struct node {
    pid_t client_pid;
    long n;
    TAILQ_ENTRY(node) nodes;
};

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
    // Инициализация очереди, в которую помещаются процессы, отправившие запрос на доступ к ресурсам
    TAILQ_HEAD(head_s, node) head;
    TAILQ_INIT(&head);

    int msqid; // IPC-дескриптор очереди сообщений

    char pathname[] = "server.c"; // Файл, использующийся для генерации ключа

    key_t key; // IPC-ключ

    long n = 0; // Значение счётчика семафора

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

    // Получаем в цикле новые запросы и пробуем дать доступ к
    // свободным ресурсам процессам, стоящим в очереди.
    while (1) {
        // Получаем запросы очередного клиента из очереди сообщений (тип сообщений = 1)
        if (msgrcv(msqid, (struct msgbuf*) &request, sizeof(request.info), 1, 0) < 0) {
            printf("Can't receive message\n");
            exit(-1);
        }

        printf("A request for access was received:\n\tProcess PID: %d; N: %ld\n", request.info.client_pid, request.info.n);

        // Определяемся, что делать, в зависимости от того, отдаёт ли процесс ресурсы или запрашивает их
        if (request.info.n >= 0) { // Если выполняется операция V(n)
            n += request.info.n;
            response.mtype = request.info.client_pid;
            msgsnd(msqid, (struct msgbuf*) &response, 0, 0);
            
            printf("Process with PID %d freed up %ld resources\n", request.info.client_pid, request.info.n);
        } else { // Если выполняется операция P(n)
            struct node* new_node = malloc(sizeof(struct node));
            new_node->client_pid = request.info.client_pid;
            new_node->n = request.info.n;
            TAILQ_INSERT_TAIL(&head, new_node, nodes); // Помещаем процесс в очередь

            printf("Process with PID %d is waiting for %ld resources...\n", request.info.client_pid, -request.info.n);
        }

        // Достаём процессы из очереди, если доступных ресурсов хватает
        struct node* first_node = TAILQ_FIRST(&head);
        while (!TAILQ_EMPTY(&head) && -first_node->n <= n) {
            long client_pid = first_node->client_pid;
            long needed_n = -first_node->n;
            
            n -= needed_n;
            TAILQ_REMOVE(&head, first_node, nodes);

            response.mtype = client_pid;
            msgsnd(msqid, (struct msgbuf*) &response, 0, 0);

            printf("Process with PID %ld is provided with needed resources\n", client_pid);
        }

        printf("Current free resources:%ld\n", n); // Печатаем, сколько ресурсов осталось на данный момент
    } // while (1)
    return 0;
}