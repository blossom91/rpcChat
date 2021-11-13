
#ifndef GuaRPCServer_h
#define GuaRPCServer_h

#include "lib/GuaBase.h"

GuaArray *
api_function_add(GuaMessage *msg);

GuaArray *
api_function_subtract(GuaMessage *msg);

GuaArray *
api_function_multiply(GuaMessage *msg);

GuaArray *
api_function_divide(GuaMessage *msg);

GuaArray *
api_function_decode(GuaMessage *msg);

GuaMessage *
handleRpcRead(GuaMessage *msg);

#endif /* GuaRPCServer_h */
