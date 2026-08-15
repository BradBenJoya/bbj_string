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

    // --- stb_string_substr ---
    stb_string src = stb_make_string("Hello World!");

    stb_string mid = stb_string_substr(&src, 6, 5);
    printf("substr(6,5): %s (expect \"World\")\n", stb_string_c_str(&mid));

    stb_string start_sub = stb_string_substr(&src, 0, 5);
    printf("substr(0,5): %s (expect \"Hello\")\n", stb_string_c_str(&start_sub));

    stb_string empty_sub = stb_string_substr(&src, 3, 0);
    printf("substr(3,0): \"%s\" (expect \"\")\n", stb_string_c_str(&empty_sub));

    stb_string full_sub = stb_string_substr(&src, 0, src.length);
    printf("substr(0,length): %s (expect \"Hello World!\")\n", stb_string_c_str(&full_sub));

    // mutating the substr must not affect src (independent buffer check)
    stb_string_append(&mid, "!!!");
    printf("after mutating substr, src is still: %s (expect \"Hello World!\")\n", stb_string_c_str(&src));

    stb_string bad_sub = stb_string_substr(&src, 5, 100);  // out of bounds
    stb_string neg_sub = stb_string_substr(&src, -1, 3);   // negative start

    // --- stb_string_find ---
    int found_mid = stb_string_find(&src, "World");
    printf("find(\"World\"): %d (expect 6)\n", found_mid);

    int found_start = stb_string_find(&src, "Hello");
    printf("find(\"Hello\"): %d (expect 0)\n", found_start);

    int found_none = stb_string_find(&src, "xyz");
    printf("find(\"xyz\"): %d (expect %d, STB_STRING_NPOS)\n", found_none, STB_STRING_NPOS);

    int found_empty = stb_string_find(&src, "");
    printf("find(\"\"): %d (expect 0)\n", found_empty);

    int found_toolong = stb_string_find(&src, "this needle is way too long to fit");
    printf("find(too long): %d (expect %d, STB_STRING_NPOS)\n", found_toolong, STB_STRING_NPOS);

    int found_end = stb_string_find(&src, "!");
    printf("find(\"!\"): %d (expect %d)\n", found_end, src.length - 1);

    // --- stb_string_copy ---
    stb_string cloned = stb_string_copy(&src);
    printf("copy: %s (expect \"Hello World!\")\n", stb_string_c_str(&cloned));

    // mutating the copy must not affect src (independent buffer check)
    stb_string_append(&cloned, "???");
    printf("after mutating copy, src is still: %s (expect \"Hello World!\")\n", stb_string_c_str(&src));
    printf("copy after mutation: %s (expect \"Hello World!???\")\n", stb_string_c_str(&cloned));

    // --- stb_string_length / stb_string_capacity ---
    stb_string cap_test = stb_make_string("abc"); // length 3, capacity 4
    printf("length: %d (expect 3)\n", stb_string_length(&cap_test));
    printf("capacity: %d (expect 4)\n", stb_string_capacity(&cap_test));

    stb_string_reserve(&cap_test, 50);
    printf("capacity after reserve(50): %d (expect >= 50)\n", stb_string_capacity(&cap_test));

    stb_string_append(&cap_test, "def");
    printf("length after append: %d (expect 6)\n", stb_string_length(&cap_test));

    stb_destroy_string(&str);
    stb_destroy_string(&a);
    stb_destroy_string(&b);
    stb_destroy_string(&c);
    stb_destroy_string(&ins);
    stb_destroy_string(&pop);
    stb_destroy_string(&popat);
    stb_destroy_string(&src);
    stb_destroy_string(&mid);
    stb_destroy_string(&start_sub);
    stb_destroy_string(&empty_sub);
    stb_destroy_string(&full_sub);
    stb_destroy_string(&bad_sub);
    stb_destroy_string(&neg_sub);
    stb_destroy_string(&cloned);
    stb_destroy_string(&cap_test);
    return 0;
}