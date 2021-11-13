#include "gm.h"
#include "lib/GuaBase.h"
#include "rpcClient.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// clang gc.c gm.c rpcClient.c lib/*.c -Ilib -pthread -fblocks -lBlocksRuntime -o c && ./c
// clang gs.c gm.c rpcServer.c lib/*.c -Ilib -pthread -fblocks -lBlocksRuntime  -o s && ./s

// typedef void (^LoginCallback)(GuaMessage *msg);
// static LoginCallback callbacks[64 * 1024];
GuaMessage *responseMessage[64 * 1024];

pthread_mutex_t msgLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dataReady = PTHREAD_COND_INITIALIZER;
static int LoginSuccess = -1;
GuaArray *arr;

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

void *
readData(void *arg) {
    // 读数据专用线程
    while (1) {
        // server发出的消息 前10字节是名字
        GuaMessage *msg = Chat_Read((int)arg);
        // 调用回调
        short sid = GuaMessage_sid(msg);

        // LoginCallback callback = callbacks[sid];

        // if (callback != NULL) {
        //     callback(msg);
        //     callbacks[sid] = NULL;
        // }

        if (GuaMessage_type(msg) == GuaMessageType_ack) {
            responseMessage[sid] = msg;
            // printf("收到 sid: %d\n", GuaMessage_sid(msg));
        } else {
            char *name = GuaMessage_name(msg);
            if (GuaMessage_type(msg) == GuaMessageType_logout) {
                // 用户退出逻辑
                printf("%s 退出聊天室\n", name);
            } else if (GuaMessage_type(msg) == GuaMessageType_login) {
                printf("%s 加入聊天室\n", name);
            } else {
                printf("%s: %s\n", name, GuaMessage_value(msg));
            }
        }
    }

    return NULL;
}

void
autoSend(void *arg) {
    while (1) {
        GuaMessage *msg = handleMsgList(GuaMessageList_pop, NULL);
        Chat_Write((int)arg, msg);
        // printf("发送 sid: %d\n", GuaMessage_sid(msg));
        free(msg);
    }
}

bool
apiLogin(int sock, GuaString *data) {
    GuaArray *list = GuaString_split(data, GuaString_new(" "));
    char *name = GuaString_value(GuaArray_get(list, 1));
    GuaMessage *msg = Chat_Login_Msg(name);
    int id = Set_Message_Sid(msg);
    handleMsgList(GuaMessageList_add, msg);

    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };

    // 每10ms查询一下是否有可发送消息
    while (1) {

        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return false;
        }
        if (responseMessage[id] != NULL) {
            GuaMessage *resMsg = responseMessage[id];
            LoginSuccess = GuaMessage_element(resMsg, 0);
            responseMessage[id] = NULL;
            free(resMsg);
            if (LoginSuccess == 0) {
                return true;
            } else {
                return false;
            }
        }
    }
}

bool
apiSendMessage(int sock, char *data) {
    GuaMessage *msg = GuaMessage_new(data);
    int id = Set_Message_Sid(msg);
    handleMsgList(GuaMessageList_add, msg);

    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };

    // 每10ms查询一下是否有可发送消息
    while (1) {

        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return false;
        }
        if (responseMessage[id] != NULL) {
            GuaMessage *resMsg = responseMessage[id];
            responseMessage[id] = NULL;
            free(resMsg);
            return true;
        }
    }
}

int
main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    const char *ip = "127.0.0.1";
    const short port = 3000;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    // 连接服务器
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("connect suc\n");

    arr = GuaArray_new();

    pthread_t pid;
    pthread_t pid2;
    pthread_create(&pid, NULL, readData, (void *)sock);
    pthread_create(&pid2, NULL, autoSend, (void *)sock);

    // 主线程读取用户输入
    while (1) {

        const int size = 100;
        char input[size];
        ssize_t n = read(STDIN_FILENO, input, size);
        input[n - 1] = 0;

        GuaString *data = GuaString_new(input);
        if (GuaString_startsWith(data, GuaString_new("/logout"))) {
            //  /logout 退出
            close(sock);
            exit(0);
        } else if (GuaString_startsWith(data, GuaString_new("/login"))) {
            // 登录消息 /name格式
            if (apiLogin(sock, data)) {
                // 成功
                printf("登录成功\n");
            } else {
                // 失败
                printf("登录失败\n");
            }
        } else if (LoginSuccess == -1) {
            printf("请先登录后发送消息\n");
        } else if (handleRpcInput(data) == 0) {
            // rpc消息
            continue;
        } else {
            // 发送普通消息
            if (apiSendMessage(sock, input)) {
                // 成功
            } else {
                // 失败
                printf("发送失败\n");
            }
        }
    }
    // 关闭socket
    close(sock);
    return 0;
}