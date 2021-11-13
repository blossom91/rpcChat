
#include "GuaType.h"
#include "GuaArray.h"
#include "GuaString.h"
#include "GuaMap.h"
#include "GuaUtils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>


#pragma mark - type declarations
struct _GuaMap {
    GuaType type;

    // buckets 用来存放节点数组
    GuaArray *buckets;

    // buckets 的数量
    long numOfBuckets;
};

struct _GuaMapNode {
    char *key;
    void *value;
};

typedef struct _GuaMapNode GuaMapNode;



#pragma mark - methods of GuaMap

GuaMap *
GuaMap_new(void) {
    GuaMap *m = malloc(sizeof(GuaMap));
    m->type = GuaTypeMap;
    
    // 不知道为什么, 但 211 必有深意
    const long n = 211;
    m->numOfBuckets = n;
    
    // 根据事先定义好的 数量 来初始化 buckets
    GuaArray *bs = GuaArray_new();
    for (long i = 0; i < n; i += 1) {
        GuaArray *b = GuaArray_new();
        GuaArray_add(bs, b);
    }

    m->buckets = bs;

    return m;
}

long
GuaMapIndex(GuaMap *map, const char *key) {
    long sum = 0;
    for (int i = 0; i < strlen(key); i += 1) {
        sum *= 10;
        char c = key[i];
        int n = toascii(c);
        sum += n;
    }

    long size = map->numOfBuckets;
    long index = sum % size;

    return index;
}

void
GuaMap_put(GuaMap *map, GuaString *key, void *value) {
    char *k = GuaString_value(key);

    // 根据 key 获取目标 bucket
    long bucketIndex = GuaMapIndex(map, k);
    GuaArray *b = GuaArray_get(map->buckets, bucketIndex);
    
    // 遍历 bucket 找到 key 相等的节点
    long bSize = GuaArray_length(b);
    for (long i = 0; i < bSize; i += 1) {
        GuaMapNode *n = GuaArray_get(b, i);
        if (strcmp(n->key, k) == 0) {
            // 如果存在则修改值并且 return
            n->value = value;
            return;
        }
    }
    
    // 不存在则创建一个新的节点
    GuaMapNode *n = malloc(sizeof(GuaMapNode));
    n->key = k;
    n->value = value;

    // 并将节点添加到 bucket
    GuaArray_add(b, n);
}

void *
GuaMap_get(GuaMap *map, GuaString *key) {
    char *k = GuaString_value(key);
    
    // 根据 key 获取目标 bucket
    long bucketIndex = GuaMapIndex(map, k);
    GuaArray *b = GuaArray_get(map->buckets, bucketIndex);
    
    // 遍历 bucket 找到 key 相等的节点
    long bSize = GuaArray_length(b);
    for (long i = 0; i < bSize; i += 1) {
        GuaMapNode *n = GuaArray_get(b, i);
        if (strcmp(n->key, k) == 0) {
            // 如果节点存在 则直接返回
            return n->value;
        }
    }
    
    // 不存在则返回 NULL
    return NULL;
}

bool
GuaMap_has(GuaMap *map, GuaString *key) {
    char *k = GuaString_value(key);
    
    // 根据 key 获取目标 bucket
    long bucketIndex = GuaMapIndex(map, k);
    GuaArray *b = GuaArray_get(map->buckets, bucketIndex);
    
    // 遍历 bucket 找到 key 相等的节点
    long bSize = GuaArray_length(b);
    for (long i = 0; i < bSize; i += 1) {
        GuaMapNode *n = GuaArray_get(b, i);
        if (strcmp(n->key, k) == 0) {
            // 如果节点存在 则直接返回 true
            return true;
        }
    }
    
    // 不存在则返回 false
    return false;
}

GuaArray *
GuaMap_keys(GuaMap *map) {
    GuaArray *ks = GuaArray_new();
    
    GuaArray *bs = map->buckets;
    long len = GuaArray_length(bs);

    // 遍历 buckets
    for (long i = 0; i < len; i += 1) {
        GuaArray *b = GuaArray_get(bs, i);
        long bLen = GuaArray_length(b);

        // 遍历 bucket
        for (long j = 0; j < bLen; j += 1) {
            // 获取 node->key
            GuaMapNode *n = GuaArray_get(b, j);
            char *k = n->key;

            // 将 key 添加到数组
            GuaArray_add(ks, k);
        }
    }

    return ks;
}

GuaArray *
GuaMap_values(GuaMap *map) {
    GuaArray *vs = GuaArray_new();

    GuaArray *bs = map->buckets;
    long len = GuaArray_length(bs);

    // 遍历 buckets
    for (long i = 0; i < len; i += 1) {
        GuaArray *b = GuaArray_get(bs, i);
        long bLen = GuaArray_length(b);

        // 遍历 bucket
        for (long j = 0; j < bLen; j += 1) {
            // 获取 node->value
            GuaMapNode *n = GuaArray_get(b, j);
            void *v = n->value;
            
            // 将 key 添加到数组
            GuaArray_add(vs, v);
        }
    }

    return vs;
}

void
GuaMap_remove(GuaMap *map, GuaString *key) {
    char *k = GuaString_value(key);
    long bucketIndex = GuaMapIndex(map, k);
    GuaArray *b = GuaArray_get(map->buckets, bucketIndex);
    
    // 遍历 bucket 找到 key 相等的节点
    long bSize = GuaArray_length(b);
    for (long i = 0; i < bSize; i += 1) {
        GuaMapNode *n = GuaArray_get(b, i);
        if (strcmp(n->key, k) == 0) {
            // 如果节点存在 则删除元素
            GuaArray_removeAtIndex(b, i);
            return;
        }
    }
}


void
GuaMap_release(GuaMap *map) {
    GuaArray *bs = map->buckets;
    
    // 遍历 buckets
    long bsLen = GuaArray_length(bs);
    for (long i = 0; i < bsLen; i += 1) {
        // free 每个 bucket
        GuaArray *b = GuaArray_get(bs, i);
        GuaArray_release(b);
    }

    // free buckets
    GuaArray_release(bs);
    
    free(map);
}

#pragma mark - tests for GuaMap

void
testMapSetGet(void) {
    GuaMap *m = GuaMap_new();
    GuaString *k = GuaString_new("name");

    GuaMap_put(m, k, "yun");
    ensure(strcmp(GuaMap_get(m, k), "yun") == 0, "testMapGet 1");
    
    GuaMap_put(m, k, "gua");
    ensure(strcmp(GuaMap_get(m, k), "gua") == 0, "testMapGet 2");
}

void
testMapHas(void) {
    GuaMap *m = GuaMap_new();
    GuaString *k1 = GuaString_new("name");

    GuaMap_put(m, k1, "yun");
    ensure(GuaMap_has(m, k1), "testMapHas 1");
    
    GuaString *k2 = GuaString_new("height");
    ensure(!GuaMap_has(m, k2), "testMapHas 2");
}

void
testMapKeys(void) {
    GuaMap *m = GuaMap_new();
    GuaString *k1 = GuaString_new("name");
    GuaString *k2 = GuaString_new("height");
    
    GuaMap_put(m, k1, "yun");

    int height = 155;
    GuaMap_put(m, k2, &height);


    GuaArray *output = GuaMap_keys(m);
    
    for (int i = 0; i < GuaArray_length(output); i += 1) {
        char *k = GuaArray_get(output, i);
        printf("testMapKeys keys[%d] = <%s>\n", i, k);
    }
    
//    GuaArray *expected = GuaArray_new();
//    GuaArrayAdd(expected, "name");
//    GuaArrayAdd(expected, "height");
//
//    ensure(GuaArrayEquals(expected, output), "testMapKeys 1");
}

void
testMapValues(void) {
    GuaMap *m = GuaMap_new();
    GuaString *k1 = GuaString_new("name");
    GuaString *k2 = GuaString_new("class");
    
    GuaMap_put(m, k1, "yun");
    GuaMap_put(m, k2, "axe4");

    GuaArray *output = GuaMap_values(m);

    for (int i = 0; i < GuaArray_length(output); i += 1) {
        char *k = GuaArray_get(output, i);
        printf("testMapValues values[%d] = <%s>\n", i, k);
    }

//    GuaArray *expected = GuaArray_new();
//    GuaArrayAdd(expected, "name");
//    GuaArrayAdd(expected, "height");
//
//    ensure(GuaArrayEquals(expected, output), "testMapKeys 1");
}


void
testMapRemove(void) {
    GuaMap *m = GuaMap_new();
    GuaString *k1 = GuaString_new("name");
    GuaString *k2 = GuaString_new("class");
    
    GuaMap_put(m, k1, "yun");
    GuaMap_put(m, k2, "axe4");
    
    GuaMap_remove(m, k1);
    GuaArray *output = GuaMap_values(m);

    for (int i = 0; i < GuaArray_length(output); i += 1) {
        char *k = GuaArray_get(output, i);
        printf("testMapRemove keys[%d] = <%s>\n", i, k);
    }
}


//int
//main(void) {
//    testMapSetGet();
//    testMapHas();
//    testMapKeys();
//    testMapValues();
//    testMapRemove();
//}
