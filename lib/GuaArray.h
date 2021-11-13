#ifndef GuaArray_h
#define GuaArray_h



#pragma mark - type declarations

struct _GuaArray;
typedef struct _GuaArray GuaArray;

#pragma mark - methods of GuaArray
GuaArray *
GuaArray_new(void);

long
GuaArray_length(GuaArray *array);

void *
GuaArray_get(GuaArray *array, long index);

void
GuaArray_set(GuaArray *array, long index, void *element);

void
GuaArray_add(GuaArray *array, void *element);

void *
GuaArray_pop(GuaArray *array);

void
GuaArray_removeAtIndex(GuaArray *array, long index);

void
GuaArray_insertAtIndex(GuaArray *array, long index, void *element);

GuaArray *
GuaArray_cut(GuaArray *array, long start, long end);

GuaArray *
GuaArray_concat(GuaArray *a1, GuaArray *a2);

long
GuaArray_find(GuaArray *array, void *element);

void
GuaArray_log(GuaArray *array);

void
GuaArray_release(GuaArray *array);

#endif /* GuaArray_h */
