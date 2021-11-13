#ifndef GuaNumber_h
#define GuaNumber_h

#include "GuaType.h"


#pragma mark - type declarations
struct _GuaNumber;
typedef struct _GuaNumber GuaNumber;



#pragma mark - methods of GuaArray
GuaNumber *
GuaNumber_newInt(int value);

GuaNumber *
GuaNumber_newFloat(float value);


int
GuaNumber_intValue(GuaNumber *n);

float
GuaNumber_floatValue(GuaNumber *n);

GuaNumberValueType
GuaNumber_type(GuaNumber *n);

void
GuaNumber_release(GuaNumber *n);

#endif /* GuaNumber_h */
