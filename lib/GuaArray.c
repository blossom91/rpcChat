#include <stdio.h>
#include <stdlib.h>
#include "GuaArray.h"
#include "GuaType.h"


#pragma mark - type declarations

struct _GuaArray {
    GuaType type;

    // 数组元素存指针
    void **elements;
    // 数组已有的元素个数
    long size;
    // 数组的容量上限
    long capacity;
};

#pragma mark - methods of GuaArray


GuaArray *
GuaArray_new(void) {
    GuaArray *ga = malloc(sizeof(struct _GuaArray));
    ga->type = GuaTypeArray;
    long capacity = 100;
    ga->size = 0;
    ga->capacity = capacity;
    ga->elements = malloc(sizeof(void *) * capacity);
    return ga;
}


void
GuaArray_expand(GuaArray *array) {
    if (array->size < array->capacity) {
        return ;
    }
    // printf("GuaArray_expand: %d\n", array->capacity);
    // 1. 扩增容量上限
    array->capacity *= 2;
    // 2. 创建一个新的 elements
    void **elements = malloc(sizeof(void *) * array->capacity);
    // 3. 把原来的 elements 的元素都放入到新的 elements
    for(long i = 0; i < array->size; i++) {
        void *e = array->elements[i];
        elements[i] = e;
    }
    // 4. free 原来的 elements
    free(array->elements);
    // 5. 替换原来的 elements
    array->elements = elements;
}


long
GuaArray_length(GuaArray *array) {
    return array->size;
}


void *
GuaArray_get(GuaArray *array, long index) {
    if (index >= array->size) {
        return NULL;
    }
    return array->elements[index];
}


void
GuaArray_set(GuaArray *array, long index, void *element) {
    array->elements[index] = element;
}


void
GuaArray_add(GuaArray *array, void *element) {
    // 当 size == capacity 时, 要扩容
    GuaArray_expand(array);
    // 数组最后位置存入指针
    long index = array->size;
    array->elements[index] = element;
    array->size = index + 1;
}


void *
GuaArray_pop(GuaArray *array) {
    long length = GuaArray_length(array);
    void *e = GuaArray_get(array, length - 1);
    // pop 出来后要置为 NULL
    GuaArray_set(array, length - 1, NULL);
    array->size -= 1;
    return e;
}


void
GuaArray_removeAtIndex(GuaArray *array, long index) {
    long i = index;
    long length = GuaArray_length(array);
    while (i < length) {
        array->elements[i] = array->elements[i + 1];
        i++;
    }
    // 最后的位置强制补一个 NULL
    array->elements[i] = NULL;
    array->size -= 1;
}


void
GuaArray_insertAtIndex(GuaArray *array, long index, void *element) {
    // 这里会新插入元素, 要注意扩容
    GuaArray_expand(array);
    // 从末尾开始, 把元素的位置都往后移动 1 位, 直到 index 的位置
    long i = array->size;
    while(i > index) {
        array->elements[i] = array->elements[i - 1];
        i--;
    }
    array->elements[i] = element;
    array->size += 1;
}


GuaArray *
GuaArray_cut(GuaArray *array, long start, long end) {
    GuaArray *new_array = GuaArray_new();
    for(long index = start; index < end; index++) {
        void *e = array->elements[index];
        GuaArray_add(new_array, e);
    }
    return new_array;
}


GuaArray *
GuaArray_concat(GuaArray *a1, GuaArray *a2) {
    long size1 = a1->size;
    long size2 = a2->size;
    GuaArray *new_array = GuaArray_new();

    // 如果两个数组的大小相加超过 初始容量, 则要进行扩容
    if ((size1 + size2) >= new_array->capacity) {
        new_array->capacity = (size1 + size2) * 2;
        new_array->elements = malloc(sizeof(void *) * (new_array->capacity));
    }

    for(long i = 0; i < size1; i++) {
        GuaArray_add(new_array, a1->elements[i]);
    }

    for(long j = 0; j < size2; j++) {
        GuaArray_add(new_array, a2->elements[j]);
    }
    return new_array;
}


// 找到 element 则返回下标, 找不到返回 -1
long
GuaArray_find(GuaArray *array, void *element) {
    long i = 0;
    long length = array->size;
    while (i < length) {
        if (array->elements[i] == element) {
            return i;
        }
        i++;
    }
    return -1;
}

void
GuaArray_release(GuaArray *array) {
    free(array->elements);
    free(array);
}
