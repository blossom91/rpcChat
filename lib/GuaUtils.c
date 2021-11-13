#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "GuaType.h"
#include "GuaNumber.h"
#include "GuaString.h"
#include "GuaArray.h"
#include "GuaMap.h"
#include "GuaUtils.h"



void
LogNumber(GuaNumber *n) {
    GuaNumberValueType type = GuaNumber_type(n);

    if (type == GuaNumberTypeInt) {
        printf("%d", GuaNumber_intValue(n));
    } else if (type == GuaNumberTypeFloat) {
        printf("%f", GuaNumber_floatValue(n));
    } else {
        printf("Unknown number type\n");
    }
}

void
LogString(GuaString *s) {
    printf("\"%s\"", GuaString_value(s));
}

void
ensure(bool result, const char *msg) {
    if (!result) {
        printf("测试失败: <%s>\n", msg);
        exit(1);
    } else {
        printf("测试成功: <%s>\n", msg);
    }
}

void
LogArray(GuaArray *array) {
    long len = GuaArray_length(array);
    
    printf("[");

    // 遍历数组内的元素
    for (long i = 0; i < len; i += 1) {
        void *v = GuaArray_get(array, i);
        
        // 对取出的元素进行判断 根据 type 打印值
        if (GuaGetType(v) == GuaTypeNumber) {
            LogNumber(v);
        } else if (GuaGetType(v) == GuaTypeString) {
            LogString(v);
        } else if (GuaGetType(v) == GuaTypeArray) {
            LogArray(v);
        }
        
        // 如果是最后一个元素则不打印 ,
        if (i < len - 1) {
            printf(", ");
        }
    }
    
    printf("]");
}

void
LogMap(GuaMap *map) {
    GuaArray *keys = GuaMap_keys(map);
    long len = GuaArray_length(keys);
    
    printf("{ ");

    // 遍历数组内的元素
    for (long i = 0; i < len; i += 1) {
        char *k = GuaArray_get(keys, i);
        void *v = GuaMap_get(map, GuaString_new(k));
        
        printf("\"%s\": ", k);

        // 对取出的元素进行判断 根据 type 打印值
        if (GuaGetType(v) == GuaTypeNumber) {
            LogNumber(v);
        } else if (GuaGetType(v) == GuaTypeString) {
            LogString(v);
        } else if (GuaGetType(v) == GuaTypeArray) {
            LogArray(v);
        } else if (GuaGetType(v) == GuaTypeMap) {
            LogMap(v);
        }
        
        // 如果是最后一个元素则不打印 ,
        if (i < len - 1) {
            printf(", ");
        }
    }
    
    printf(" }");
}


void
GuaLog(void *element) {
    if (GuaGetType(element) == GuaTypeNumber) {
        LogNumber(element);
    } else if (GuaGetType(element) == GuaTypeArray) {
        LogArray(element);
    } else if (GuaGetType(element) == GuaTypeString) {
        LogString(element);
    } else if (GuaGetType(element) == GuaTypeMap) {
        LogMap(element);
    }
    
    printf("\n");
}
