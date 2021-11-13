#ifndef GuaChat_h
#define GuaChat_h

#include "lib/GuaBase.h"

#pragma mark - type declarations

struct _GuaMessage;
struct _GuaMessageHeader;
struct _payload;
typedef struct _payload payload;
typedef struct _GuaMessage GuaMessage;
typedef struct _GuaMessageHeader GuaMessageHeader;

typedef struct {
    // 如果是广播，它是 -1
    int clientSocket;
    // 消息
    GuaMessage *message;
} GuaMsgWorkerMessage;

typedef enum {
    GuaMessageType_login = 0,
    GuaMessageType_message,
    GuaMessageType_logout,
    //
    GuaMessageType_ack = 100,
};

enum {
    GuaMessageList_add = 0,
    GuaMessageList_pop,
};

typedef enum {
    GuaPayloadTypeString = 1,
    GuaPayloadTypeNumber = 2,
    GuaPayloadTypeArray = 3,
} GuaPayloadType;

#pragma mark - methods of GuaChat

GuaMessage *
GuaMessage_new(const char *s);

GuaMessage *
Chat_Read(int fd);

void
Chat_Write(int fd, GuaMessage *s);

GuaMessage *
Chat_Login_Msg(const char *name);

GuaMessage *
Chat_Logout_Msg();

GuaMessage *
Chat_ACK_Msg(GuaMessage *s);

int
Set_Message_Sid(GuaMessage *s);

GuaMessage *
Set_Message_Login_Success(GuaMessage *s);

GuaMessage *
Set_Message_Payload(GuaMessage *s, GuaArray *l);

GuaMessage *
Set_Message_Type(GuaMessage *s, int type);

GuaMessage *
Set_Message_Name(GuaMessage *s, const char *name);

unsigned short
GuaMessage_type(GuaMessage *s);

unsigned int
GuaMessage_length(GuaMessage *s);

unsigned short
GuaMessage_sid(GuaMessage *s);

char *
GuaMessage_value(GuaMessage *s);

char *
GuaMessage_name(GuaMessage *s);

void *
GuaMessage_element(GuaMessage *s, int index);

GuaArray *
GuaMessage_payload(GuaMessage *s);

#endif /* GuaChat_h */
