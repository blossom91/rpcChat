#ifndef GuaString_h
#define GuaString_h

#include <stdbool.h>
#include "GuaType.h"
#include "GuaArray.h"

/*
这里用 GuaString 演示一下如何在 c 中实现自己的数据结构

 1, 头文件中做好函数声明，但是没有任何内部的信息
 2, 用户只能通过头文件中的函数来操作对象
*/

#pragma mark - type declarations
struct _GuaString;
typedef struct _GuaString GuaString;



#pragma mark - methods of GuaString

GuaString *
GuaString_new(const char *s);

long
GuaString_length(GuaString *s);

GuaString *
GuaString_cut(GuaString *s, long start, long end);

bool
GuaString_equals(GuaString *s1, GuaString *s2);

bool
GuaString_startsWith(GuaString *s1, GuaString *s2);

bool
GuaString_endsWith(GuaString *s1, GuaString *s2);

long
GuaString_find(GuaString *s1, GuaString *s2);

GuaString *
GuaString_concat(GuaString *s1, GuaString *s2);

char
GuaString_charAt(GuaString *s, long index);

GuaString *
GuaString_strip(GuaString *s);

GuaString *
GuaString_replace(GuaString *s, GuaString *source, GuaString *target);

GuaArray *
GuaString_split(GuaString *s, GuaString *delimiter);

char *
GuaString_value(GuaString *s);

void
GuaString_release(GuaString *s);

#endif /* GuaString_h */
