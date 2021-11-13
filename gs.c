#include "gm.h"
#include "lib/GuaBase.h"
#include "rpcServer.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int MaxCount = 100;
int clientFds[10000];

int clientIndex = 0;
int onlineCount = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t msgLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dataReady = PTHREAD_COND_INITIALIZER;

GuaArray *arr;

char *
handleDecode(char *source, int offset) {
    char *base = "abcdefghijklmnopqrstuvwxyz";
    int baselen = 26;
    int len = strlen(source);
    char *res = malloc(len);

    for (int i = 0; i < len; i++) {
        char n = source[i];
        for (int j = 0; j < baselen; j++) {
            if (n == base[j]) {
                int index = (j - offset) % baselen;
                if (index < 0) {
                    index += baselen;
                } else if (index > baselen) {
                    index += baselen;
                }
                res[i] = base[index];
            }
        }
    }
    return res;
}

void *
handleMsgList(int type, void *arg) {
    GuaMessage *msg = NULL;
    pthread_mutex_lock(&msgLock);
    if (type == GuaMessageList_add) {
        GuaArray_insertAtIndex(arr, 0, arg);
        pthread_mutex_unlock(&msgLock);
        pthread_cond_signal(&dataReady);
    } else if (type == GuaMessageList_pop) {
        if (GuaArray_length(arr) < 1) {
            pthread_cond_wait(&dataReady, &msgLock);
        }
        msg = GuaArray_pop(arr);
        pthread_mutex_unlock(&msgLock);
    }
    return msg;
}

int
addOnlineCount(int n) {
    pthread_mutex_lock(&lock);
    onlineCount += n;
    pthread_mutex_unlock(&lock);
}

int
openSocket(unsigned short port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);
    bind(s, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(s, 5);
    printf("listening at port %d\n", port);
    return s;
}

void
sendWorker(void *arg) {
    while (1) {
        GuaMsgWorkerMessage *workerMsg = handleMsgList(GuaMessageList_pop, NULL);
        if (workerMsg->clientSocket != -1) {
            Chat_Write(workerMsg->clientSocket, workerMsg->message);
        } else {
            for (int i = 0; i < clientIndex; i++) {
                int fd = clientFds[i];
                if (fd != -1) {
                    Chat_Write(fd, workerMsg->message);
                }
            }
        }
        free(workerMsg);
    }
}

void
addGuaWorker(GuaMessage *msg, int fd) {
    GuaMsgWorkerMessage *workerMsg = malloc(sizeof(GuaMsgWorkerMessage));
    workerMsg->clientSocket = fd;
    workerMsg->message = msg;
    handleMsgList(GuaMessageList_add, workerMsg);
}

void *
readData(void *arg) {
    addOnlineCount(1);

    int fd = clientFds[(int)arg];
    char name[10];

    while (1) {
        // server 收到的消息都是没有name的
        GuaMessage *msg = Chat_Read(fd);

        GuaMessage *rpcMsg = handleRpcRead(msg);
        if (rpcMsg != NULL) {
            // rpc消息
            addGuaWorker(rpcMsg, fd);
            continue;
        }

        GuaMessage *ackMsg = Chat_ACK_Msg(msg);
        if (GuaMessage_type(msg) == GuaMessageType_login) {
            char *n = GuaMessage_name(msg);
            strncpy(name, n, strlen(n));
            printf("Login Message form (%s) online(%d)\n", name, onlineCount);
            // 重写一下msg内容 回复0 确认登录成功
            Set_Message_Login_Success(ackMsg);
        } else if (GuaMessage_type(msg) == GuaMessageType_logout) {
            // 用户退出逻辑
            clientFds[(int)arg] = -1;
        }

        addGuaWorker(ackMsg, fd);
        Set_Message_Name(msg, name);
        addGuaWorker(msg, -1);

        if (GuaMessage_type(msg) == GuaMessageType_logout) {
            addOnlineCount(-1);
            printf("Leave Message form (%s) online(%d)\n", name, onlineCount);
            close(fd);
            break;
        }
    }
    return NULL;
}

int
main(int argc, const char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    unsigned short port = 3000;
    int s = openSocket(port);

    struct sockaddr_in client;
    int size = sizeof(struct sockaddr_in);

    arr = GuaArray_new();

    pthread_t msgWorker;
    pthread_create(&msgWorker, NULL, sendWorker, (void *)index);

    while (1) {

        int clientSocket = accept(s, (struct sockaddr *)&client, (socklen_t *)&size);
        // 检测是否可连接
        if (onlineCount > MaxCount) {
            close(clientSocket);
            continue;
        }

        clientFds[clientIndex] = clientSocket;
        int index = clientIndex;
        pthread_t pid;
        pthread_create(&pid, NULL, readData, (void *)index);
        clientIndex++;
    }

    return 0;
}
