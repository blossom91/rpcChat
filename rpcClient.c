
#include "gm.h"
#include "lib/GuaBase.h"
#include <stdlib.h>
#include <unistd.h>

extern GuaMessage *responseMessage[64 * 1024];

typedef enum {
    GuaMessageType_add = 101,
    GuaMessageType_subtract,
    GuaMessageType_multiply,
    GuaMessageType_divide,
    GuaMessageType_decode,
};

int
api_add(GuaString *data) {
    GuaMessage *msg = GuaMessage_new(NULL);
    int id = Set_Message_Sid(msg);
    GuaArray *sp = GuaString_split(data, GuaString_new(" "));
    char *m1 = GuaString_value(GuaArray_get(sp, 1));
    char *m2 = GuaString_value(GuaArray_get(sp, 2));

    int mi1 = atoi(m1);
    int mi2 = atoi(m2);
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(mi1));

    GuaArray_add(l, GuaNumber_newInt(mi2));
    Set_Message_Payload(msg, l);
    Set_Message_Type(msg, GuaMessageType_add);
    handleMsgList(GuaMessageList_add, msg);
    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };
    while (1) {
        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return NULL;
        }
        if (responseMessage[id] != NULL) {
            return id;
        }
    }
}

int
api_subtract(GuaString *data) {
    GuaMessage *msg = GuaMessage_new(NULL);
    int id = Set_Message_Sid(msg);
    GuaArray *sp = GuaString_split(data, GuaString_new(" "));
    char *m1 = GuaString_value(GuaArray_get(sp, 1));
    char *m2 = GuaString_value(GuaArray_get(sp, 2));

    int mi1 = atoi(m1);
    int mi2 = atoi(m2);
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(mi1));

    GuaArray_add(l, GuaNumber_newInt(mi2));
    Set_Message_Payload(msg, l);
    Set_Message_Type(msg, GuaMessageType_subtract);
    handleMsgList(GuaMessageList_add, msg);
    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };
    while (1) {
        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return NULL;
        }
        if (responseMessage[id] != NULL) {
            return id;
        }
    }
}

int
api_multiply(GuaString *data) {
    GuaMessage *msg = GuaMessage_new(NULL);
    int id = Set_Message_Sid(msg);
    GuaArray *sp = GuaString_split(data, GuaString_new(" "));
    char *m1 = GuaString_value(GuaArray_get(sp, 1));
    char *m2 = GuaString_value(GuaArray_get(sp, 2));

    int mi1 = atoi(m1);
    int mi2 = atoi(m2);
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(mi1));

    GuaArray_add(l, GuaNumber_newInt(mi2));
    Set_Message_Payload(msg, l);
    Set_Message_Type(msg, GuaMessageType_multiply);
    handleMsgList(GuaMessageList_add, msg);
    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };
    while (1) {
        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return NULL;
        }
        if (responseMessage[id] != NULL) {
            return id;
        }
    }
}

int
api_divide(GuaString *data) {
    GuaMessage *msg = GuaMessage_new(NULL);
    int id = Set_Message_Sid(msg);
    GuaArray *sp = GuaString_split(data, GuaString_new(" "));
    char *m1 = GuaString_value(GuaArray_get(sp, 1));
    char *m2 = GuaString_value(GuaArray_get(sp, 2));

    int mi1 = atoi(m1);
    int mi2 = atoi(m2);
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(mi1));

    GuaArray_add(l, GuaNumber_newInt(mi2));
    Set_Message_Payload(msg, l);
    Set_Message_Type(msg, GuaMessageType_divide);
    handleMsgList(GuaMessageList_add, msg);
    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };
    while (1) {
        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return NULL;
        }
        if (responseMessage[id] != NULL) {
            return id;
        }
    }
}

int
api_decode(GuaString *data) {
    GuaMessage *msg = GuaMessage_new(NULL);
    int id = Set_Message_Sid(msg);
    GuaArray *sp = GuaString_split(data, GuaString_new(" "));
    char *m1 = GuaString_value(GuaArray_get(sp, 1));
    char *m2 = GuaString_value(GuaArray_get(sp, 2));

    int mi2 = atoi(m2);
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaString_new(m1));

    GuaArray_add(l, GuaNumber_newInt(mi2));
    Set_Message_Payload(msg, l);
    Set_Message_Type(msg, GuaMessageType_decode);
    handleMsgList(GuaMessageList_add, msg);
    int timeout = 5 * 1000;
    struct timespec time10ms = {
        .tv_sec = 0,
        .tv_nsec = 10 * 1000 * 1000,
    };
    while (1) {
        nanosleep(&time10ms, NULL);
        timeout -= 10;
        if (timeout <= 0) {
            return NULL;
        }
        if (responseMessage[id] != NULL) {
            return id;
        }
    }
}

int
handleRpcInput(GuaString *data) {
    int id;
    if (GuaString_startsWith(data, GuaString_new("+"))) {
        id = api_add(data);
    } else if (GuaString_startsWith(data, GuaString_new("-"))) {
        id = api_subtract(data);
    } else if (GuaString_startsWith(data, GuaString_new("*"))) {
        id = api_multiply(data);
    } else if (GuaString_startsWith(data, GuaString_new("/"))) {
        id = api_divide(data);
    } else if (GuaString_startsWith(data, GuaString_new("decode"))) {
        id = api_decode(data);
    } else {
        return 1;
    }

    GuaMessage *resMsg = responseMessage[id];
    GuaArray *l = GuaMessage_payload(resMsg);
    GuaLog(GuaArray_get(l, 0));
    responseMessage[id] = NULL;
    free(resMsg);
    return 0;
}
