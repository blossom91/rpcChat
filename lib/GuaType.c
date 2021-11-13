#include "GuaType.h"

#pragma mark - methods of GuaType
GuaType
GuaGetType(void *v) {
    struct {
        GuaType type;
    } *n = v;

    GuaType type = n->type;
    return type;
}
