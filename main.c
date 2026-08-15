#include <stdio.h>
#define STB_STRING_IMPLEMENTATION
#include "stb_string.h"

int main(void)
{
    stb_string str = stb_make_string("Hello World!");

    *stb_string_at(&str, 0) = 'Z';
    stb_string_append(&str, " String in C!");
    stb_print_string(&str);

    printf("c_str: %s\n", stb_string_c_str(&str));

    stb_string_reserve(&str, 100);
    int len_before = str.length;
    stb_string_append(&str, " more text");
    printf("length after append post-reserve: %d (started at %d)\n", str.length, len_before);

    stb_string_clear(&str);
    printf("after clear, c_str: \"%s\" (length = %d)\n", stb_string_c_str(&str), str.length);

    stb_destroy_string(&str);
    return 0;
}