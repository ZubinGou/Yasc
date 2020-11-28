int printf(const char *, ...);

// Line comment
int main(void) {
    int foo_int = 0x0114514;
    float a = .1e1;
    float foo_float = 123.f;
    float bar_float = .251e+3l;
    char *foo_char = "\"";
    int i = 100lu;
    i <<= 2;
    /* This
     * is
     * a block comment */
    printf("你好！おはよう!%d\n", _ident32);
    return 0;
}
