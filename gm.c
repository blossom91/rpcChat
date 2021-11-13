#include "gm.h"
#include "lib/GuaBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#pragma mark - type declarations

struct _GuaMessageHeader {
    unsigned int length;
    unsigned short type; // 1 send 2 login 3 logout
    unsigned short sid;
};

struct _GuaMessage {
    GuaMessageHeader header;
    GuaArray *payload;
};

#pragma mark - methods of GuaChat

int sid = 0;

char *
TransPayload(GuaMessage *msg) {
    GuaArray *l = msg->payload;
    // 假定最大数据长度 255
    char *data = malloc(255);
    int offset = 0;
    // type
    data[offset++] = 3;
    // 数组长度
    int arrLen = GuaArray_length(l);
    data[offset++] = arrLen;
    for (int i = 0; i < arrLen; i++) {
        // 判断类型
        void *e = GuaArray_get(l, i);
        GuaType type = GuaGetType(e);
        // 目前只有str 先处理这些
        if (type == GuaTypeString) {
            data[offset++] = GuaPayloadTypeString;
            int strlen = GuaString_length(e) + 1;
            data[offset++] = strlen;
            memcpy(data + offset, GuaString_value(e), strlen);
            offset += strlen;
        } else if (type == GuaTypeNumber) {
            data[offset++] = GuaPayloadTypeNumber;
            int num = GuaNumber_intValue(e);
            memcpy(data + offset, &num, 4);
            offset += 4;
        } else {
            // 先不处理
        }
    }
    msg->header.length = offset;
    return data;
}

GuaArray *
ReadPayload(const char *payload) {
    GuaArray *l = GuaArray_new();
    int offset = 1;
    int arrLen = payload[offset++];
    for (int i = 0; i < arrLen; i++) {
        GuaPayloadType type = payload[offset++];

        if (type == GuaPayloadTypeString) {
            int strlen = payload[offset++];
            char *data = malloc(strlen);
            memcpy(data, payload + offset, strlen);
            GuaArray_add(l, GuaString_new(data));
            offset += strlen;
        } else if (type == GuaPayloadTypeNumber) {
            int n;
            memcpy(&n, payload + offset, 4);
            GuaArray_add(l, GuaNumber_newInt(n));
            offset += 4;
        } else {
            // 先不解析
        }
    }
    return l;
}

GuaMessage *
GuaMessage_new(const char *s) {
    GuaMessage *msg = malloc(sizeof(GuaMessage));
    // 默认普通消息
    msg->header.type = GuaMessageType_message;
    GuaArray *l = GuaArray_new();
    if (s != NULL) {
        GuaArray_add(l, GuaString_new(s));
    }
    msg->payload = l;
    return msg;
}

GuaMessage *
Chat_Read(int fd) {
    GuaMessage *msg = GuaMessage_new(NULL);
    ssize_t n = read(fd, &msg->header, sizeof(GuaMessageHeader));
    if (n < 1) {
        // 算用户退出,要广播
        msg->header.type = GuaMessageType_logout;
        return msg;
    } else {
        char *data = malloc(msg->header.length);
        read(fd, data, msg->header.length);
        GuaArray *payload = ReadPayload(data);
        msg->payload = payload;
        return msg;
    }
}

void
Chat_Write(int fd, GuaMessage *s) {
    char *data = TransPayload(s);
    write(fd, &s->header, sizeof(GuaMessageHeader));
    write(fd, data, s->header.length);
}

GuaMessage *
Chat_Login_Msg(const char *name) {
    GuaMessage *msg = GuaMessage_new(name);
    msg->header.type = GuaMessageType_login;
    return msg;
}

GuaMessage *
Chat_Logout_Msg() {
    GuaMessage *msg = GuaMessage_new(NULL);
    msg->header.type = GuaMessageType_logout;
    return msg;
}

GuaMessage *
Chat_ACK_Msg(GuaMessage *s) {
    GuaMessage *msg = GuaMessage_new(NULL);
    msg->header.type = GuaMessageType_ack;
    msg->header.sid = s->header.sid;
    return msg;
}

GuaMessage *
Set_Message_Name(GuaMessage *s, const char *name) {
    // name 长度为10
    if (name != NULL) {
        GuaArray *l = s->payload;
        GuaArray_insertAtIndex(l, 0, GuaString_new(name));
    }
    return s;
}

GuaMessage *
Set_Message_Type(GuaMessage *s, int type) {
    s->header.type = type;
    return s;
}

int
Set_Message_Sid(GuaMessage *s) {
    s->header.sid = sid++;
    return s->header.sid;
}

GuaMessage *
Set_Message_Payload(GuaMessage *s, GuaArray *l) {
    s->payload = l;
    return s;
}

GuaMessage *
Set_Message_Login_Success(GuaMessage *s) {
    GuaArray *l = GuaArray_new();
    GuaArray_add(l, GuaNumber_newInt(0));
    s->payload = l;
    return s;
}

unsigned short
GuaMessage_type(GuaMessage *s) {
    return s->header.type;
}

unsigned int
GuaMessage_length(GuaMessage *s) {
    return s->header.length;
}

unsigned short
GuaMessage_sid(GuaMessage *s) {
    return s->header.sid;
}

char *
GuaMessage_value(GuaMessage *s) {
    GuaArray *l = s->payload;
    GuaString *m = GuaArray_get(l, 1);
    char *msg = GuaString_value(m);
    return msg;
}

char *
GuaMessage_name(GuaMessage *s) {
    GuaArray *l = s->payload;
    GuaString *m = GuaArray_get(l, 0);
    char *msg = GuaString_value(m);
    return msg;
}

void *
GuaMessage_element(GuaMessage *s, int index) {
    GuaArray *l = s->payload;
    GuaString *m = GuaArray_get(l, index);
    GuaType type = GuaGetType(m);
    char *msg;
    if (type == GuaTypeString) {
        msg = GuaString_value(m);
    } else if (type == GuaTypeNumber) {
        msg = GuaNumber_intValue(m);
    }

    return msg;
}

GuaArray *
GuaMessage_payload(GuaMessage *s) {
    GuaArray *l = s->payload;
    return l;
}