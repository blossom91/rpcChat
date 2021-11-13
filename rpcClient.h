
#ifndef GuaRPCClient_h
#define GuaRPCClient_h

#include "lib/GuaBase.h"

int
api_add(GuaString *data);

int
api_subtract(GuaString *data);

int
api_multiply(GuaString *data);

int
api_divide(GuaString *data);

int
api_decode(GuaString *data);

int
handleRpcInput(GuaString *data);

#endif /* GuaRPCClient_h */
