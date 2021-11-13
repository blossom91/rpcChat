#ifndef GuaUtils_h
#define GuaUtils_h

#include <stdbool.h>

void
GuaLog(void *element);

void
ensure(bool result, const char *msg);

#endif /* GuaUtils_h */
