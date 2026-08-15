#include <stdio.h>
#include <string.h>
#include <assert.h>

#define STB_STRING_IMPLEMENTATION
#include "../stb_string.h"

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
    stb_string s = stb_make_string("hello");
    CHECK_STR(stb_string_c_str(&s), "hello");
    CHECK(stb_string_length(&s) == 5);
    CHECK(stb_string_capacity(&s) == 6);
    stb_destroy_string(&s);
    CHECK(stb_string_c_str(&s) == NULL);
    CHECK(stb_string_length(&s) == 0);

    stb_string null_s = stb_make_string(NULL);
    CHECK(stb_string_c_str(&null_s) == NULL);
    stb_destroy_string(&null_s);
}

static void test_at(void)
{
    stb_string s = stb_make_string("abc");
    CHECK(*stb_string_at(&s, 0) == 'a');
    CHECK(*stb_string_at(&s, 2) == 'c');
    CHECK(stb_string_at(&s, 3) == NULL);
    CHECK(stb_string_at(&s, -1) == NULL);
    CHECK(stb_string_at(NULL, 0) == NULL);

    *stb_string_at(&s, 0) = 'z';
    CHECK_STR(stb_string_c_str(&s), "zbc");

    stb_destroy_string(&s);
}

static void test_append(void)
{
    stb_string s = stb_make_string("foo");
    stb_string_append(&s, "bar");
    CHECK_STR(stb_string_c_str(&s), "foobar");
    CHECK(stb_string_length(&s) == 6);

    // force growth past initial capacity repeatedly
    for (int i = 0; i < 50; i++) stb_string_append(&s, "x");
    CHECK(stb_string_length(&s) == 56);
    CHECK(stb_string_capacity(&s) >= 57);

    stb_string_append(&s, NULL); // should no-op, not crash
    CHECK(stb_string_length(&s) == 56);

    stb_destroy_string(&s);
}

static void test_insert(void)
{
    stb_string s = stb_make_string("Hello !");
    stb_string_insert(&s, 6, "World");
    CHECK_STR(stb_string_c_str(&s), "Hello World!");

    stb_string_insert(&s, 0, ">> ");
    CHECK_STR(stb_string_c_str(&s), ">> Hello World!");

    stb_string_insert(&s, s.length, " <<");
    CHECK_STR(stb_string_c_str(&s), ">> Hello World! <<");

    int len_before = s.length;
    stb_string_insert(&s, -1, "bad");
    stb_string_insert(&s, s.length + 1, "bad");
    CHECK(s.length == len_before); // both rejected, no change

    stb_destroy_string(&s);
}

static void test_pop(void)
{
    stb_string s = stb_make_string("abcde");
    stb_string_pop(&s);
    CHECK_STR(stb_string_c_str(&s), "abcd");
    stb_destroy_string(&s);

    stb_string s2 = stb_make_string("abcde");
    stb_string_pop_at(&s2, 1);
    CHECK_STR(stb_string_c_str(&s2), "acde");
    stb_string_pop_at(&s2, 0);
    CHECK_STR(stb_string_c_str(&s2), "cde");
    stb_string_pop_at(&s2, s2.length - 1);
    CHECK_STR(stb_string_c_str(&s2), "cd");

    int len_before = s2.length;
    stb_string_pop_at(&s2, -1);
    stb_string_pop_at(&s2, s2.length);
    CHECK(s2.length == len_before);

    stb_destroy_string(&s2);
}

static void test_clear(void)
{
    stb_string s = stb_make_string("something");
    stb_string_clear(&s);
    CHECK_STR(stb_string_c_str(&s), "");
    CHECK(stb_string_length(&s) == 0);
    CHECK(stb_string_empty(&s));
    stb_destroy_string(&s);
}

static void test_reserve(void)
{
    stb_string s = stb_make_string("abc");
    int cap_before = stb_string_capacity(&s);
    stb_string_reserve(&s, 100);
    CHECK(stb_string_capacity(&s) >= 100);

    // reserving smaller than current capacity must not shrink it
    int cap_after = stb_string_capacity(&s);
    stb_string_reserve(&s, 10);
    CHECK(stb_string_capacity(&s) == cap_after);
    (void)cap_before;

    stb_destroy_string(&s);
}

static void test_cmp(void)
{
    stb_string a = stb_make_string("apple");
    stb_string b = stb_make_string("apple");
    stb_string c = stb_make_string("banana");

    CHECK(stb_string_cmp(&a, &b) == 0);
    CHECK(stb_string_cmp(&a, &c) < 0);
    CHECK(stb_string_cmp(&c, &a) > 0);

    stb_destroy_string(&a);
    stb_destroy_string(&b);
    stb_destroy_string(&c);
}

static void test_find(void)
{
    stb_string s = stb_make_string("Hello World!");

    CHECK(stb_string_find(&s, "World") == 6);
    CHECK(stb_string_find(&s, "Hello") == 0);
    CHECK(stb_string_find(&s, "xyz") == STB_STRING_NPOS);
    CHECK(stb_string_find(&s, "") == 0);
    CHECK(stb_string_find(&s, "this needle is way too long to fit") == STB_STRING_NPOS);
    CHECK(stb_string_find(&s, "!") == s.length - 1);

    stb_destroy_string(&s);
}

static void test_copy(void)
{
    stb_string s = stb_make_string("Hello World!");
    stb_string clone = stb_string_copy(&s);
    CHECK_STR(stb_string_c_str(&clone), "Hello World!");

    stb_string_append(&clone, "???");
    CHECK_STR(stb_string_c_str(&s), "Hello World!");        // source untouched
    CHECK_STR(stb_string_c_str(&clone), "Hello World!???"); // clone changed

    stb_destroy_string(&s);
    stb_destroy_string(&clone);
}

static void test_substr(void)
{
    stb_string s = stb_make_string("Hello World!");

    stb_string mid = stb_string_substr(&s, 6, 5);
    CHECK_STR(stb_string_c_str(&mid), "World");

    stb_string start = stb_string_substr(&s, 0, 5);
    CHECK_STR(stb_string_c_str(&start), "Hello");

    stb_string empty = stb_string_substr(&s, 3, 0);
    CHECK_STR(stb_string_c_str(&empty), "");

    stb_string full = stb_string_substr(&s, 0, s.length);
    CHECK_STR(stb_string_c_str(&full), "Hello World!");

    stb_string_append(&mid, "!!!");
    CHECK_STR(stb_string_c_str(&s), "Hello World!"); // source untouched

    stb_string bad = stb_string_substr(&s, 5, 100);
    CHECK(stb_string_c_str(&bad) == NULL);

    stb_string neg = stb_string_substr(&s, -1, 3);
    CHECK(stb_string_c_str(&neg) == NULL);

    stb_destroy_string(&s);
    stb_destroy_string(&mid);
    stb_destroy_string(&start);
    stb_destroy_string(&empty);
    stb_destroy_string(&full);
    stb_destroy_string(&bad);
    stb_destroy_string(&neg);
}

static void test_null_safety(void)
{
    // none of these should crash
    CHECK(stb_string_length(NULL) == 0);
    CHECK(stb_string_capacity(NULL) == 0);
    CHECK(stb_string_empty(NULL) != 0);
    CHECK(stb_string_c_str(NULL) == NULL);
    stb_destroy_string(NULL);
    stb_print_string(NULL);
    stb_string_clear(NULL);
    stb_string_reserve(NULL, 10);
    stb_string_pop(NULL);
    stb_string_pop_at(NULL, 0);

    stb_string bad_copy = stb_string_copy(NULL);
    CHECK(stb_string_c_str(&bad_copy) == NULL);

    stb_string bad_sub = stb_string_substr(NULL, 0, 1);
    CHECK(stb_string_c_str(&bad_sub) == NULL);

    CHECK(stb_string_find(NULL, "x") == STB_STRING_NPOS);
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