#include <stdlib.h>

#include "GuaType.h"
#include "GuaNumber.h"


#pragma mark - type declarations
struct _GuaNumber {
    GuaType type;
    GuaNumberValueType valueType;
    // void *value;
    union {
        int valueInt;
        float valueFloat;
    } value;
};



#pragma mark - methods of GuaArray
GuaNumber *
GuaNumber_newInt(int value) {
    GuaNumber *n = malloc(sizeof(GuaNumber));
    n->type = GuaTypeNumber;
    n->valueType = GuaNumberTypeInt;
    n->value.valueInt = value;
    return n;
}

GuaNumber *
GuaNumber_newFloat(float value) {
    GuaNumber *n = malloc(sizeof(GuaNumber));
    n->type = GuaTypeNumber;
    n->valueType = GuaNumberTypeFloat;
    n->value.valueFloat = value;
    return n;
}


int
GuaNumber_intValue(GuaNumber *n) {
    return n->value.valueInt;
}

float
GuaNumber_floatValue(GuaNumber *n)  {
    return n->value.valueFloat;
}

GuaNumberValueType
GuaNumber_type(GuaNumber *n) {
    return n->valueType;
}

void
GuaNumber_release(GuaNumber *n) {
    free(n);
}
