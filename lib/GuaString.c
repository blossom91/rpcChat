#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GuaUtils.h"

#include "GuaType.h"
#include "GuaArray.h"
#include "GuaString.h"



#pragma mark - type declarations
struct _GuaString {
    GuaType type;
    char *str;
    long length;
};



#pragma mark - methods of GuaString
GuaString *
GuaString_new(const char *s) {
    size_t len = strlen(s);
    char *str = malloc((sizeof(char) + 1) * len);
    // c 的原始内置函数 strcpy 是不检查内存越界的
    // 可以被利用来一直写数据进行缓冲区溢出攻击
    // 所以现在不要用了，用 strncpy
    strncpy(str, s, len + 1);

    // 一般而言，在 c 语言中有 2 种方案做自定义数据结构
    // 下面是方案 1，学校应该教这种
    // 这种方案用指针传递对象
    // 返回值就是复制一个指针也就是 8 字节
    GuaString *gs = malloc(sizeof(GuaString));
    gs->type = GuaTypeString;
    gs->str = str;
    gs->length = len;

    return gs;
}

long
GuaString_length(GuaString *s) {
    return s->length;
}

GuaString *
GuaString_cut(GuaString *s, long start, long end) {
    long len = end - start + 1;

    char *cutted = malloc(sizeof(char) * len);

    // 循环拷贝字符
    for (int i = 0; i < len; i++) {
        cutted[i] = s->str[start + i];
    }

    cutted[len - 1] = '\0';

    return GuaString_new(cutted);
}

bool
GuaString_equals(GuaString *s1, GuaString *s2) {
    long len1 = GuaString_length(s1);
    long len2 = GuaString_length(s2);

    // 如果两个字符串长度不相等则直接返回 false
    if (len1 != len2) {
        return false;
    }

    // 循环比较每一个字符
    for (int i = 0; i < len1; i++) {
        if (s1->str[i] != s2->str[i]) {
            return false;
        }
    }

    return true;
}

bool
GuaString_startsWith(GuaString *s1, GuaString *s2) {
    long len = GuaString_length(s2);
    
    // 切分出与 s2 相等字符串长度的字符串, 用以比对
    GuaString *s = GuaString_cut(s1, 0, len);

    return GuaString_equals(s, s2);
}

bool
GuaString_endsWith(GuaString *s1, GuaString *s2) {
    long len1 = GuaString_length(s1);
    long len2 = GuaString_length(s2);
    
    // 想要截取字符串的开始位置
    long start = len1 - len2;
    
    // 想要截取字符串的结束位置
    long end = start + len2;

    // 根据 start 和 end 截取出子字符串用来和 s2 比对
    GuaString *s = GuaString_cut(s1, start, end);

    return GuaString_equals(s, s2);
}

long
GuaString_find(GuaString *s1, GuaString *s2) {
    long len1 = GuaString_length(s1);
    long len2 = GuaString_length(s2);
    
    // 遍历 s1
    for (long i = 0; i < len1; i += 1) {
        long start = i;
        long end = start + len2;
        
        // 截取出与 s2 相同长度的字符串
        GuaString *s = GuaString_cut(s1, start, end);
        // printf("cutted <%s>\n", s->str);
        
        // 和 s2 进行比较
        if (GuaString_equals(s, s2)) {
            // 如果和 s2 相等, 则返回当前下标
            return i;
        }
    }
    
    // 如果循环走完了都没有找到, 则返回 -1
    return -1;
}

GuaString *
GuaString_concat(GuaString *s1, GuaString *s2) {
    // 求出 s1 s2 的长度
    long len1 = GuaString_length(s1);
    long len2 = GuaString_length(s2);

    // 拼接后的新字符串 r 的长度要多 1 个用于放末尾的 '\0'
    long len3 = len1 + len2 + 1;
    char *r = malloc(sizeof(char) * len3);

    // 循环把 s1 里的字符都复制到 s3 中
    for (int i = 0; i < len1; i++) {
        r[i] = s1->str[i];
    }

    // 循环把 s2 里的字符都复制到 s3 后面
    for (int i = 0; i < len2; i++) {
        r[i + len1] = s2->str[i];
    }

    // 最后一位要设置为 '\0' 才是一个合格的字符串
    r[len3 - 1] = '\0';

    GuaString *s3 = GuaString_new(r);
    return s3;
}

char
GuaString_charAt(GuaString *s, long index) {
    return s->str[index];
}

bool
_isSpace(char c) {
    const char *spaces = " \t\r\n";
    for (int i = 0; i < strlen(spaces); i += 1) {
        if (c == spaces[i]) {
            return true;
        }
    }

    return false;
}

GuaString *
GuaString_strip(GuaString *s) {
    long len = GuaString_length(s);
    long start = 0;
    long end = GuaString_length(s) - 1;

    // 从前往后遍历, 直到当前字符不是 空白字符
    while (start < len) {
        char c = GuaString_charAt(s, start);
        if (!_isSpace(c)) {
            break;
        }

        start += 1;
    }


    // 如果 start 和 len 相等, 说明整个字符串只有空白字符
    if (start == len) {
        return GuaString_new("");
    }


    // 从后往前遍历, 直到当前字符不是 空白字符
    while (end >= 0) {
        char c = GuaString_charAt(s, end);
        if (!_isSpace(c)) {
            break;
        }
        
        end -= 1;
    }

    return GuaString_cut(s, start, end + 1);
}

GuaString *
GuaString_replace(GuaString *s, GuaString *source, GuaString *target) {
    long len = GuaString_length(s);
    long sourceLen = GuaString_length(source);
    
    // 创建一个空的字符串用作存储替换后的数据
    char *replaced = malloc(sizeof(char));
    
    // 遍历整个字符串
    long i = 0;
    while (i < len) {
        // 截取与 source 同样长度的字符串
        long start = i;
        long end = start + sourceLen;
        GuaString *subString = GuaString_cut(s, start, end);

        // 与 source 进行比较
        if (GuaString_equals(subString, source)) {
            // 如果与 source 相等, 则将 target 拼接到 replaced, i 也要加上 source 的长度
            strncat(replaced, target->str, target->length);
            i += sourceLen;
        } else {
            // 否则直接将字符拼接到 replaced
            char c = GuaString_charAt(s, i);
            strncat(replaced, &c, 1);
            i += 1;
        }
    }
    
    return GuaString_new(replaced);
}

GuaArray *
GuaString_split(GuaString *s, GuaString *delimiter) {
    long i = 0;
    long len = GuaString_length(s);
    long dLen = GuaString_length(delimiter);

    // 用来保存结果的数组
    GuaArray *parts = GuaArray_new();
    // printf("len: (%ld)\n", len);    
    while (i < len) {
        long end = GuaString_find(GuaString_cut(s, i, len), delimiter);

        // 如果不存在分隔符，则将整个字符串作为分割后的一部分
        if (end == -1) {
            end = len;
        }
        // 将切分出来的子字符串存入数组
        GuaString *cutted = GuaString_cut(s, i, i + end);
        GuaArray_add(parts, cutted);
        i += end + dLen;
    }
    
    return parts;
}

char *
GuaString_value(GuaString *s) {
   return s->str;
}

void
GuaString_release(GuaString *s) {
    free(s);
}



// #pragma mark - tests for GuaString

// void
// 测试字符串创建(void) {
//     GuaString *s1 = GuaString_new("");
//     char *o1 = s1->str;
//     char *e1 = "";
    
//     ensure(strcmp(o1, e1) == 0 && s1->length == 0, "测试字符串创建 1");
    
    
//     GuaString *s2 = GuaString_new("123456");
//     char *o2 = s2->str;
//     char *e2 = "123456";
    
//     ensure(strcmp(o2, e2) == 0 && s1->length == 6, "测试字符串创建 2");
// }

// void
// 测试字符串相等(void) {
//     GuaString *s1 = GuaString_new("");
//     GuaString *s2 = GuaString_new("");
//     bool o1 = GuaString_equals(s1, s2);
//     bool e1 = true;
    
//     ensure(o1 == e1, "测试字符串相等 1");



//     GuaString *s3 = GuaString_new("123456");
//     GuaString *s4 = GuaString_new("123456");
//     bool o2 = GuaString_equals(s3, s4);
//     bool e2 = true;
    
//     ensure(o2 == e2, "测试字符串相等 2");
// }

// void
// 测试字符串长度(void) {
//     GuaString *s1 = GuaString_new("123456");
//     long o1 = GuaString_length(s1);
//     long e1 = 6;

//     ensure(o1 == e1, "测试字符串长度 1");


//     GuaString *s2 = GuaString_new("");
//     long o2 = GuaString_length(s2);
//     long e2 = 0;

//     ensure(o2 == e2, "测试字符串长度 2");
// }

// void
// 测试字符串是否以XX开头(void) {
//     GuaString *s1 = GuaString_new("123456");
//     GuaString *s2 = GuaString_new("123");
//     bool o1 = GuaString_startsWith(s1, s2);
//     bool e1 = true;
//     ensure(o1 == e1, "测试字符串开头1");
    
//     GuaString *s3 = GuaString_new("123456");
//     GuaString *s4 = GuaString_new("234");
//     bool o2 = GuaString_startsWith(s3, s4);
//     bool e2 = false;
//     ensure(o2 == e2, "测试字符串开头2");
// }

// void
// 测试字符串是否以XX结尾(void) {
//     GuaString *s1 = GuaString_new("123456");
//     GuaString *s2 = GuaString_new("456");
//     bool o1 = GuaString_endsWith(s1, s2);
//     bool e1 = true;
//     ensure(o1 == e1, "测试字符串结尾1");
    
//     GuaString *s3 = GuaString_new("123456");
//     GuaString *s4 = GuaString_new("345");
//     bool o2 = GuaString_endsWith(s3, s4);
//     bool e2 = false;
//     ensure(o2 == e2, "测试字符串结尾2");
// }

// void
// 测试字符串查找(void) {
//     GuaString *s1 = GuaString_new("123456");
//     GuaString *s2 = GuaString_new("345");
//     long o1 = GuaString_find(s1, s2);
//     long e1 = 2;

//     ensure(o1 == e1, "测试字符串查找 1");


//     GuaString *s3 = GuaString_new("123456");
//     GuaString *s4 = GuaString_new("6");
//     long o2 = GuaString_find(s3, s4);
//     long e2 = 5;
    
//     ensure(o2 == e2, "测试字符串查找 2");


//     GuaString *s5 = GuaString_new("123456");
//     GuaString *s6 = GuaString_new("1");
//     long o3 = GuaString_find(s5, s6);
//     long e3 = 0;
    
//     ensure(o3 == e3, "测试字符串查找 3");
// }

// void
// 测试字符串拼接(void) {
//     GuaString *s1 = GuaString_new("123");
//     GuaString *s2 = GuaString_new("456");
//     GuaString *o1 = GuaString_concat(s1, s2);
//     GuaString *e1 = GuaString_new("123456");

//     ensure(GuaString_equals(o1, e1), "测试字符串拼接 1");
    
//     GuaString *s3 = GuaString_new("2345");
//     GuaString *s4 = GuaString_new("6");
//     GuaString *o2 = GuaString_concat(s3, s4);
//     GuaString *e2 = GuaString_new("23456");

//     ensure(GuaString_equals(o2, e2), "测试字符串拼接 2");
// }

// void
// 测试字符串去左右空白符(void) {
//     GuaString *s1 = GuaString_new("  123 ");
//     GuaString *o1 = GuaString_strip(s1);
//     GuaString *e1 = GuaString_new("123");

//     ensure(GuaString_equals(o1, e1), "测试字符串去左右空白符 1");



//     GuaString *s2 = GuaString_new("\r123\n\t");
//     GuaString *o2 = GuaString_strip(s2);
//     GuaString *e2 = GuaString_new("123");

//     ensure(GuaString_equals(o2, e2), "测试字符串去左右空白符 2");



//     GuaString *s3 = GuaString_new("   ");
//     GuaString *o3 = GuaString_strip(s3);
//     GuaString *e3 = GuaString_new("");

//     ensure(GuaString_equals(o3, e3), "测试字符串去左右空白符 1");
// }

// void
// 测试字符串替换(void) {
//     GuaString *s1 = GuaString_new("123");
//     GuaString *o1 = GuaString_replace(s1, GuaString_new("3"), GuaString_new("4"));
//     GuaString *e1 = GuaString_new("124");

//     ensure(GuaString_equals(o1, e1), "测试字符串替换 1");
    
    
//     GuaString *s2 = GuaString_new("123");
//     GuaString *o2 = GuaString_replace(s2, GuaString_new("3"), GuaString_new("444"));
//     GuaString *e2 = GuaString_new("12444");

//     ensure(GuaString_equals(o2, e2), "测试字符串替换 2");
    
    
//     GuaString *s3 = GuaString_new("111");
//     GuaString *o3 = GuaString_replace(s3, GuaString_new("1"), GuaString_new("2"));
//     GuaString *e3 = GuaString_new("222");

//     ensure(GuaString_equals(o3, e3), "测试字符串替换 3");
// }

//int
//main() {
//    测试字符串创建();
//    测试字符串相等();
//    测试字符串长度();
//    测试字符串是否以XX开头();
//    测试字符串是否以XX结尾();
//    测试字符串查找();
//    测试字符串拼接();
//    测试字符串去左右空白符();
//    测试字符串替换();
//
//
//    
//    printf("字符串测试完成\n");
//    return 0;
//}
