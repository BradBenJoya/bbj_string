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

    // --- stb_string_cmp ---
    stb_string a = stb_make_string("apple");
    stb_string b = stb_make_string("apple");
    stb_string c = stb_make_string("banana");

    printf("cmp(a, b) = %d (expect 0)\n", stb_string_cmp(&a, &b));
    printf("cmp(a, c) = %d (expect < 0)\n", stb_string_cmp(&a, &c));
    printf("cmp(c, a) = %d (expect > 0)\n", stb_string_cmp(&c, &a));

    // --- stb_string_insert ---
    stb_string ins = stb_make_string("Hello !");
    stb_string_insert(&ins, 6, "World");
    printf("insert middle: %s (expect \"Hello World!\")\n", stb_string_c_str(&ins));

    stb_string_insert(&ins, 0, ">> ");
    printf("insert at start: %s (expect \">> Hello World!\")\n", stb_string_c_str(&ins));

    stb_string_insert(&ins, ins.length, " <<");
    printf("insert at end: %s (expect \">> Hello World! <<\")\n", stb_string_c_str(&ins));

    stb_string_insert(&ins, -1, "bad");   // should print bounds error, no crash
    stb_string_insert(&ins, ins.length + 1, "bad"); // should print bounds error, no crash

    // --- stb_string_pop ---
    stb_string pop = stb_make_string("abcde");
    stb_string_pop(&pop);
    printf("pop: %s (expect \"abcd\")\n", stb_string_c_str(&pop));

    // --- stb_string_pop_at ---
    stb_string popat = stb_make_string("abcde");
    stb_string_pop_at(&popat, 1); // remove 'b'
    printf("pop_at(1): %s (expect \"acde\")\n", stb_string_c_str(&popat));

    stb_string_pop_at(&popat, 0); // remove 'a'
    printf("pop_at(0): %s (expect \"cde\")\n", stb_string_c_str(&popat));

    stb_string_pop_at(&popat, popat.length - 1); // remove last char 'e'
    printf("pop_at(last): %s (expect \"cd\")\n", stb_string_c_str(&popat));

    stb_string_pop_at(&popat, -1);              // should print bounds error, no crash
    stb_string_pop_at(&popat, popat.length);     // should print bounds error, no crash

    stb_destroy_string(&str);
    stb_destroy_string(&a);
    stb_destroy_string(&b);
    stb_destroy_string(&c);
    stb_destroy_string(&ins);
    stb_destroy_string(&pop);
    stb_destroy_string(&popat);
    return 0;
}