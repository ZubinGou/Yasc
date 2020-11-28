#include <stdio.h>

// Line comment
int fibonacci(int i) {
    if (i <= 1) {
        return 1;
    }
    return fibonacci(i-1) + fibonacci(i-2);
}
/* This
     * is
     * a block comment */
int main()
{
    int i;
    @ i = 0; // 非法字符
    while (i <= 10) {
        printf("fibonacci(%2d) = %d\n", i, fibonacci(i));
        i = i + 1;
    }
    char c = '\n // 未闭合字符
    printf("你好！おはよう!");
    return 0;
}