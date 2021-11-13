
#include "gm.h"
#include "lib/GuaBase.h"
#include <stdlib.h>
#include <unistd.h>

typedef enum {
    GuaMessageType_add = 101,
    GuaMessageType_subtract,
    GuaMessageType_multiply,
    GuaMessageType_divide,
    GuaMessageType_decode,
};

GuaArray *
api_function_add(GuaMessage *msg) {
    int mi1 = GuaMessage_element(msg, 0);
    int mi2 = GuaMessage_element(msg, 1);
    int res = mi1 + mi2;

    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(res));
    return l;
}

GuaArray *
api_function_subtract(GuaMessage *msg) {
    int mi1 = GuaMessage_element(msg, 0);
    int mi2 = GuaMessage_element(msg, 1);
    int res = mi1 - mi2;

    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(res));
    return l;
}

GuaArray *
api_function_multiply(GuaMessage *msg) {
    int mi1 = GuaMessage_element(msg, 0);
    int mi2 = GuaMessage_element(msg, 1);
    int res = mi1 * mi2;

    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(res));
    return l;
}

GuaArray *
api_function_divide(GuaMessage *msg) {
    int mi1 = GuaMessage_element(msg, 0);
    int mi2 = GuaMessage_element(msg, 1);
    int res = mi1 / mi2;

    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(res));
    return l;
}

GuaArray *
api_function_decode(GuaMessage *msg) {
    char *res = GuaMessage_element(msg, 0);
    int offset = GuaMessage_element(msg, 1);
    char *d = res;
    while (*d != '\0') {
        *d++ -= offset;
    }
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaString_new(res));
    return l;
}

GuaMessage *
handleRpcRead(GuaMessage *msg) {
    GuaArray *l;
    if (GuaMessage_type(msg) == GuaMessageType_add) {
        l = api_function_add(msg);
    } else if (GuaMessage_type(msg) == GuaMessageType_subtract) {
        l = api_function_subtract(msg);
    } else if (GuaMessage_type(msg) == GuaMessageType_multiply) {
        l = api_function_multiply(msg);
    } else if (GuaMessage_type(msg) == GuaMessageType_divide) {
        l = api_function_divide(msg);
    } else if (GuaMessage_type(msg) == GuaMessageType_decode) {
        l = api_function_decode(msg);
    } else {
        return NULL;
    }

    GuaMessage *ackMsg = Chat_ACK_Msg(msg);
    Set_Message_Payload(ackMsg, l);
    return ackMsg;
}
