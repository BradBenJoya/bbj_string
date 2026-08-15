#include <stdio.h>
#define STB_STRING_IMPLEMENTATION
#include "stb_string.h"

int main(void)
{
    stb_string str = stb_make_string("Hello World!");
    *stb_string_at(&str, 0) = 'Z';
    stb_string_append(&str, " String in C!");
    stb_print_string(&str);
    stb_destroy_string(&str);
    return 0;
}
