/* bbj_string.h - v1.1 - public domain string library
   A dynamic, owning, null-terminated string type for C.

   Do this:
      #define BBJ_STRING_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create
   the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #define BBJ_STRING_IMPLEMENTATION
   #include "bbj_string.h"

   Optionally define BBJ_STRING_STATIC before the include to make
   all functions and the implementation private to that file.

   NOTES

   - length/capacity are stored as `int`. Strings longer than
     INT_MAX are not supported; this is a deliberate choice to
     keep the struct small and match the signed-index style used
     throughout the API (BBJ_STRING_NPOS is -1, not a huge
     unsigned value). If you need to hold multi-gigabyte strings,
     this library isn't the right tool as-is.

   - not thread-safe. A single bbj_string must not be accessed
     from more than one thread at a time without external
     synchronization; two threads may safely use two different
     bbj_strings concurrently.

   - functions that take a bbj_string* or a char* parameter check
     for NULL and fail safely (returning NULL / BBJ_STRING_NPOS /
     an empty string / doing nothing, and printing to stderr)
     rather than crashing. This costs a branch per call; if you
     never pass NULL and want to skip the checks, that's a
     reasonable thing to strip out yourself.

   VERSION HISTORY

     1.1  (2026-08)
          - added NULL-input guards to every function
          - added bbj_string_length / bbj_string_capacity accessors
          - added bbj_string_copy, bbj_string_substr, bbj_string_find
          - added bbj_string_insert, bbj_string_pop, bbj_string_pop_at
          - reorganized into stb-style header/implementation layout
            (BBJSDEF, extern "C", public-domain license block)
     1.0  (2026-08)
          - initial version: make/destroy/print, at, append, clear,
            reserve, c_str, cmp

   LICENSE
     This software is dual-licensed to the public domain and under
     the following license: you are granted a perpetual, irrevocable
     license to copy, modify, publish, and distribute this file as
     you see fit.
*/

#ifndef UNTITLED_BBJ_STRING_H
#define UNTITLED_BBJ_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BBJ_STRING_STATIC
#define BBJSDEF static
#else
#define BBJSDEF extern
#endif

#define BBJ_STRING_NPOS (-1)

typedef struct
{
    char* data;
    int length;
    int capacity;
} bbj_string;

BBJSDEF bbj_string  bbj_make_string(char* data);           // owned copy of data; s.data is NULL if data is NULL or malloc failed
BBJSDEF void        bbj_destroy_string(bbj_string* s);
BBJSDEF void        bbj_print_string(bbj_string* s);
BBJSDEF const char* bbj_string_c_str(bbj_string* s);       // NULL if s or s->data is NULL
BBJSDEF int         bbj_string_length(bbj_string* s);      // 0 if s is NULL
BBJSDEF int         bbj_string_capacity(bbj_string* s);    // 0 if s is NULL
BBJSDEF int         bbj_string_empty(bbj_string* s);       // nonzero if s is NULL or length == 0

BBJSDEF char*       bbj_string_at(bbj_string* s, int index);              // NULL if s is NULL or index out of bounds
BBJSDEF void        bbj_string_append(bbj_string* s, char* data);
BBJSDEF void        bbj_string_insert(bbj_string* s, int index, char* data);
BBJSDEF void        bbj_string_pop(bbj_string* s);
BBJSDEF void        bbj_string_pop_at(bbj_string* s, int index);
BBJSDEF void        bbj_string_clear(bbj_string* s);
BBJSDEF void        bbj_string_reserve(bbj_string* s, unsigned int capacity);

BBJSDEF int         bbj_string_cmp(bbj_string* s1, bbj_string* s2);       // like strcmp: 0/<0/>0
BBJSDEF int         bbj_string_find(bbj_string* s, char* data);           // index of match, or BBJ_STRING_NPOS

BBJSDEF bbj_string  bbj_string_copy(bbj_string* s);                          // owned copy; NULL data on failure
BBJSDEF bbj_string  bbj_string_substr(bbj_string* s, int start, int length); // owned copy; NULL data on failure

#ifdef __cplusplus
}
#endif

#endif // UNTITLED_BBJ_STRING_H


#ifdef BBJ_STRING_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

BBJSDEF bbj_string bbj_make_string(char* data)
{
    bbj_string s;
    s.data = NULL;
    s.length = 0;
    s.capacity = 0;

    if (!data)
    {
        fprintf(stderr, "bbj_make_string: data is NULL\n");
        return s;
    }

    s.length = (int)strlen(data);
    s.capacity = s.length + 1; // +1 for null terminator
    s.data = malloc(s.capacity);
    if (!s.data)
    {
        fprintf(stderr, "bbj_make_string: malloc failed\n");
        s.length = 0;
        s.capacity = 0;
        return s;
    }
    memcpy(s.data, data, s.capacity);
    return s;
}

BBJSDEF void bbj_destroy_string(bbj_string* s)
{
    if (!s) return;
    free(s->data);
    s->data = NULL;
    s->length = 0;
    s->capacity = 0;
}

BBJSDEF void bbj_print_string(bbj_string* s)
{
    if (!s || !s->data)
    {
        fprintf(stderr, "bbj_print_string: string is NULL or empty\n");
        return;
    }
    printf("%s\n", s->data);
}

BBJSDEF const char* bbj_string_c_str(bbj_string* s)
{
    if (!s) return NULL;
    return s->data;
}

BBJSDEF int bbj_string_length(bbj_string* s)
{
    if (!s) return 0;
    return s->length;
}

BBJSDEF int bbj_string_capacity(bbj_string* s)
{
    if (!s) return 0;
    return s->capacity;
}

BBJSDEF int bbj_string_empty(bbj_string* s)
{
    if (!s) return 1;
    return s->length == 0;
}

BBJSDEF char* bbj_string_at(bbj_string* s, int index)
{
    if (!s)
    {
        fprintf(stderr, "bbj_string_at: string is NULL\n");
        return NULL;
    }
    if (index >= s->length || index < 0)
    {
        fprintf(stderr, "bbj_string_at: index out of bounds at %d\n", index);
        return NULL;
    }
    return &s->data[index];
}

BBJSDEF void bbj_string_append(bbj_string* s, char* data)
{
    if (!s || !data)
    {
        fprintf(stderr, "bbj_string_append: string or data is NULL\n");
        return;
    }

    int append_len = (int)strlen(data);

    if (s->length + append_len + 1 > s->capacity)
    {
        int new_capacity = (s->length + append_len + 1) * 2;
        char* new_data = realloc(s->data, new_capacity);
        if (!new_data)
        {
            fprintf(stderr, "bbj_string_append: realloc failed\n");
            return;
        }
        s->data = new_data;
        s->capacity = new_capacity;
    }

    memcpy(s->data + s->length, data, append_len + 1); // includes null terminator
    s->length += append_len;
}

BBJSDEF void bbj_string_insert(bbj_string* s, int index, char* data)
{
    if (!s || !data)
    {
        fprintf(stderr, "bbj_string_insert: string or data is NULL\n");
        return;
    }
    if (index < 0 || index > s->length)
    {
        fprintf(stderr, "bbj_string_insert: index out of bounds at %d\n", index);
        return;
    }

    int insert_len = (int)strlen(data);

    if (s->length + insert_len + 1 > s->capacity)
    {
        int new_capacity = (s->length + insert_len + 1) * 2;
        char* new_data = realloc(s->data, new_capacity);
        if (!new_data)
        {
            fprintf(stderr, "bbj_string_insert: realloc failed\n");
            return;
        }
        s->data = new_data;
        s->capacity = new_capacity;
    }

    memmove(s->data + index + insert_len, s->data + index, s->length - index + 1);
    memcpy(s->data + index, data, insert_len);

    s->length += insert_len;
}

BBJSDEF void bbj_string_pop(bbj_string* s)
{
    if (!s)
    {
        fprintf(stderr, "bbj_string_pop: string is NULL\n");
        return;
    }
    if (s->length == 0) return;
    s->length -= 1;
    s->data[s->length] = '\0';
}

BBJSDEF void bbj_string_pop_at(bbj_string* s, int index)
{
    if (!s)
    {
        fprintf(stderr, "bbj_string_pop_at: string is NULL\n");
        return;
    }
    if (s->length == 0) return;
    if (index < 0 || index >= s->length)
    {
        fprintf(stderr, "bbj_string_pop_at: index out of bounds at %d\n", index);
        return;
    }

    memmove(s->data + index, s->data + index + 1, s->length - index);
    s->length -= 1;
}

BBJSDEF void bbj_string_clear(bbj_string* s)
{
    if (!s || !s->data)
    {
        fprintf(stderr, "bbj_string_clear: string is NULL or empty\n");
        return;
    }
    s->length = 0;
    s->data[0] = '\0';
}

BBJSDEF void bbj_string_reserve(bbj_string* s, unsigned int capacity)
{
    if (!s)
    {
        fprintf(stderr, "bbj_string_reserve: string is NULL\n");
        return;
    }
    if (capacity <= (unsigned int)s->capacity)
    {
        return;
    }

    char* new_data = realloc(s->data, capacity);
    if (!new_data)
    {
        fprintf(stderr, "bbj_string_reserve: realloc failed\n");
        return;
    }

    s->data = new_data;
    s->capacity = (int)capacity;
}

BBJSDEF int bbj_string_cmp(bbj_string* s1, bbj_string* s2)
{
    if (!s1 || !s2 || !s1->data || !s2->data)
    {
        fprintf(stderr, "bbj_string_cmp: string is NULL or empty\n");
        return s1 == s2 ? 0 : (s1 ? 1 : -1);
    }
    return strcmp(s1->data, s2->data);
}

BBJSDEF int bbj_string_find(bbj_string* s, char* find)
{
    if (!s || !find)
    {
        fprintf(stderr, "bbj_string_find: string or needle is NULL\n");
        return BBJ_STRING_NPOS;
    }

    int find_len = (int)strlen(find);

    if (find_len == 0) return 0;
    if (find_len > s->length) return BBJ_STRING_NPOS;

    for (int i = 0; i <= s->length - find_len; i++)
    {
        if (strncmp(s->data + i, find, find_len) == 0)
        {
            return i;
        }
    }

    return BBJ_STRING_NPOS;
}

BBJSDEF bbj_string bbj_string_copy(bbj_string* s)
{
    bbj_string str;
    str.data = NULL;
    str.length = 0;
    str.capacity = 0;

    if (!s || !s->data)
    {
        fprintf(stderr, "bbj_string_copy: string is NULL or empty\n");
        return str;
    }

    str.length = s->length;
    str.capacity = s->length + 1;
    str.data = malloc(str.capacity);
    if (!str.data)
    {
        fprintf(stderr, "bbj_string_copy: malloc failed\n");
        str.length = 0;
        str.capacity = 0;
        return str;
    }
    memcpy(str.data, s->data, str.capacity); // includes null terminator
    return str;
}

BBJSDEF bbj_string bbj_string_substr(bbj_string* s, int start, int length)
{
    bbj_string str;
    str.data = NULL;
    str.length = 0;
    str.capacity = 0;

    if (!s || !s->data)
    {
        fprintf(stderr, "bbj_string_substr: string is NULL or empty\n");
        return str;
    }

    if (start < 0 || length < 0 || start + length > s->length)
    {
        fprintf(stderr, "bbj_string_substr: range out of bounds (start=%d, length=%d)\n", start, length);
        return str;
    }

    str.capacity = length + 1; // +1 for null terminator
    str.data = malloc(str.capacity);
    if (!str.data)
    {
        fprintf(stderr, "bbj_string_substr: malloc failed\n");
        str.capacity = 0;
        return str;
    }

    memcpy(str.data, s->data + start, length);
    str.data[length] = '\0';
    str.length = length;

    return str;
}

#ifdef __cplusplus
}
#endif

#endif // BBJ_STRING_IMPLEMENTATION