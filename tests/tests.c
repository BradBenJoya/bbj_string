/* stb_string.h - v1.1 - public domain string library
   A dynamic, owning, null-terminated string type for C.

   Do this:
      #define STB_STRING_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create
   the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #define STB_STRING_IMPLEMENTATION
   #include "stb_string.h"

   Optionally define STB_STRING_STATIC before the include to make
   all functions and the implementation private to that file.

   NOTES

   - length/capacity are stored as `int`. Strings longer than
     INT_MAX are not supported; this is a deliberate choice to
     keep the struct small and match the signed-index style used
     throughout the API (STB_STRING_NPOS is -1, not a huge
     unsigned value). If you need to hold multi-gigabyte strings,
     this library isn't the right tool as-is.

   - not thread-safe. A single stb_string must not be accessed
     from more than one thread at a time without external
     synchronization; two threads may safely use two different
     stb_strings concurrently.

   - functions that take a stb_string* or a char* parameter check
     for NULL and fail safely (returning NULL / STB_STRING_NPOS /
     an empty string / doing nothing, and printing to stderr)
     rather than crashing. This costs a branch per call; if you
     never pass NULL and want to skip the checks, that's a
     reasonable thing to strip out yourself.

   VERSION HISTORY

     1.1  (2026-08)
          - added NULL-input guards to every function
          - added stb_string_length / stb_string_capacity accessors
          - added stb_string_copy, stb_string_substr, stb_string_find
          - added stb_string_insert, stb_string_pop, stb_string_pop_at
          - reorganized into stb-style header/implementation layout
            (STBSDEF, extern "C", public-domain license block)
     1.0  (2026-08)
          - initial version: make/destroy/print, at, append, clear,
            reserve, c_str, cmp

   LICENSE
     This software is dual-licensed to the public domain and under
     the following license: you are granted a perpetual, irrevocable
     license to copy, modify, publish, and distribute this file as
     you see fit.
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BBJ_STRING_IMPLEMENTATION
#include "../bbj_string.h"

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(cond) do { \
    tests_run++; \
    if (!(cond)) { \
        tests_failed++; \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    } \
} while (0)

#define CHECK_STR(actual, expected) CHECK(strcmp((actual), (expected)) == 0)

static void test_make_and_destroy(void)
{
    bbj_string s = bbj_make_string("hello");
    CHECK_STR(bbj_string_c_str(&s), "hello");
    CHECK(bbj_string_length(&s) == 5);
    CHECK(bbj_string_capacity(&s) == 6);
    bbj_destroy_string(&s);
    CHECK(bbj_string_c_str(&s) == NULL);
    CHECK(bbj_string_length(&s) == 0);

    bbj_string null_s = bbj_make_string(NULL);
    CHECK(bbj_string_c_str(&null_s) == NULL);
    bbj_destroy_string(&null_s);
}

static void test_at(void)
{
    bbj_string s = bbj_make_string("abc");
    CHECK(*bbj_string_at(&s, 0) == 'a');
    CHECK(*bbj_string_at(&s, 2) == 'c');
    CHECK(bbj_string_at(&s, 3) == NULL);
    CHECK(bbj_string_at(&s, -1) == NULL);
    CHECK(bbj_string_at(NULL, 0) == NULL);

    *bbj_string_at(&s, 0) = 'z';
    CHECK_STR(bbj_string_c_str(&s), "zbc");

    bbj_destroy_string(&s);
}

static void test_append(void)
{
    bbj_string s = bbj_make_string("foo");
    bbj_string_append(&s, "bar");
    CHECK_STR(bbj_string_c_str(&s), "foobar");
    CHECK(bbj_string_length(&s) == 6);

    // force growth past initial capacity repeatedly
    for (int i = 0; i < 50; i++) bbj_string_append(&s, "x");
    CHECK(bbj_string_length(&s) == 56);
    CHECK(bbj_string_capacity(&s) >= 57);

    bbj_string_append(&s, NULL); // should no-op, not crash
    CHECK(bbj_string_length(&s) == 56);

    bbj_destroy_string(&s);
}

static void test_insert(void)
{
    bbj_string s = bbj_make_string("Hello !");
    bbj_string_insert(&s, 6, "World");
    CHECK_STR(bbj_string_c_str(&s), "Hello World!");

    bbj_string_insert(&s, 0, ">> ");
    CHECK_STR(bbj_string_c_str(&s), ">> Hello World!");

    bbj_string_insert(&s, s.length, " <<");
    CHECK_STR(bbj_string_c_str(&s), ">> Hello World! <<");

    int len_before = s.length;
    bbj_string_insert(&s, -1, "bad");
    bbj_string_insert(&s, s.length + 1, "bad");
    CHECK(s.length == len_before); // both rejected, no change

    bbj_destroy_string(&s);
}

static void test_pop(void)
{
    bbj_string s = bbj_make_string("abcde");
    bbj_string_pop(&s);
    CHECK_STR(bbj_string_c_str(&s), "abcd");
    bbj_destroy_string(&s);

    bbj_string s2 = bbj_make_string("abcde");
    bbj_string_pop_at(&s2, 1);
    CHECK_STR(bbj_string_c_str(&s2), "acde");
    bbj_string_pop_at(&s2, 0);
    CHECK_STR(bbj_string_c_str(&s2), "cde");
    bbj_string_pop_at(&s2, s2.length - 1);
    CHECK_STR(bbj_string_c_str(&s2), "cd");

    int len_before = s2.length;
    bbj_string_pop_at(&s2, -1);
    bbj_string_pop_at(&s2, s2.length);
    CHECK(s2.length == len_before);

    bbj_destroy_string(&s2);
}

static void test_clear(void)
{
    bbj_string s = bbj_make_string("something");
    bbj_string_clear(&s);
    CHECK_STR(bbj_string_c_str(&s), "");
    CHECK(bbj_string_length(&s) == 0);
    CHECK(bbj_string_empty(&s));
    bbj_destroy_string(&s);
}

static void test_reserve(void)
{
    bbj_string s = bbj_make_string("abc");
    int cap_before = bbj_string_capacity(&s);
    bbj_string_reserve(&s, 100);
    CHECK(bbj_string_capacity(&s) >= 100);

    // reserving smaller than current capacity must not shrink it
    int cap_after = bbj_string_capacity(&s);
    bbj_string_reserve(&s, 10);
    CHECK(bbj_string_capacity(&s) == cap_after);
    (void)cap_before;

    bbj_destroy_string(&s);
}

static void test_cmp(void)
{
    bbj_string a = bbj_make_string("apple");
    bbj_string b = bbj_make_string("apple");
    bbj_string c = bbj_make_string("banana");

    CHECK(bbj_string_cmp(&a, &b) == 0);
    CHECK(bbj_string_cmp(&a, &c) < 0);
    CHECK(bbj_string_cmp(&c, &a) > 0);

    bbj_destroy_string(&a);
    bbj_destroy_string(&b);
    bbj_destroy_string(&c);
}

static void test_find(void)
{
    bbj_string s = bbj_make_string("Hello World!");

    CHECK(bbj_string_find(&s, "World") == 6);
    CHECK(bbj_string_find(&s, "Hello") == 0);
    CHECK(bbj_string_find(&s, "xyz") == BBJ_STRING_NPOS);
    CHECK(bbj_string_find(&s, "") == 0);
    CHECK(bbj_string_find(&s, "this needle is way too long to fit") == BBJ_STRING_NPOS);
    CHECK(bbj_string_find(&s, "!") == s.length - 1);

    bbj_destroy_string(&s);
}

static void test_copy(void)
{
    bbj_string s = bbj_make_string("Hello World!");
    bbj_string clone = bbj_string_copy(&s);
    CHECK_STR(bbj_string_c_str(&clone), "Hello World!");

    bbj_string_append(&clone, "???");
    CHECK_STR(bbj_string_c_str(&s), "Hello World!");        // source untouched
    CHECK_STR(bbj_string_c_str(&clone), "Hello World!???"); // clone changed

    bbj_destroy_string(&s);
    bbj_destroy_string(&clone);
}

static void test_substr(void)
{
    bbj_string s = bbj_make_string("Hello World!");

    bbj_string mid = bbj_string_substr(&s, 6, 5);
    CHECK_STR(bbj_string_c_str(&mid), "World");

    bbj_string start = bbj_string_substr(&s, 0, 5);
    CHECK_STR(bbj_string_c_str(&start), "Hello");

    bbj_string empty = bbj_string_substr(&s, 3, 0);
    CHECK_STR(bbj_string_c_str(&empty), "");

    bbj_string full = bbj_string_substr(&s, 0, s.length);
    CHECK_STR(bbj_string_c_str(&full), "Hello World!");

    bbj_string_append(&mid, "!!!");
    CHECK_STR(bbj_string_c_str(&s), "Hello World!"); // source untouched

    bbj_string bad = bbj_string_substr(&s, 5, 100);
    CHECK(bbj_string_c_str(&bad) == NULL);

    bbj_string neg = bbj_string_substr(&s, -1, 3);
    CHECK(bbj_string_c_str(&neg) == NULL);

    bbj_destroy_string(&s);
    bbj_destroy_string(&mid);
    bbj_destroy_string(&start);
    bbj_destroy_string(&empty);
    bbj_destroy_string(&full);
    bbj_destroy_string(&bad);
    bbj_destroy_string(&neg);
}

static void test_null_safety(void)
{
    // none of these should crash
    CHECK(bbj_string_length(NULL) == 0);
    CHECK(bbj_string_capacity(NULL) == 0);
    CHECK(bbj_string_empty(NULL) != 0);
    CHECK(bbj_string_c_str(NULL) == NULL);
    bbj_destroy_string(NULL);
    bbj_print_string(NULL);
    bbj_string_clear(NULL);
    bbj_string_reserve(NULL, 10);
    bbj_string_pop(NULL);
    bbj_string_pop_at(NULL, 0);

    bbj_string bad_copy = bbj_string_copy(NULL);
    CHECK(bbj_string_c_str(&bad_copy) == NULL);

    bbj_string bad_sub = bbj_string_substr(NULL, 0, 1);
    CHECK(bbj_string_c_str(&bad_sub) == NULL);

    CHECK(bbj_string_find(NULL, "x") == BBJ_STRING_NPOS);
}

int main(void)
{
    test_make_and_destroy();
    test_at();
    test_append();
    test_insert();
    test_pop();
    test_clear();
    test_reserve();
    test_cmp();
    test_find();
    test_copy();
    test_substr();
    test_null_safety();

    printf("\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}
