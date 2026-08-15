#ifndef UNTITLED_STB_STRING_H
#define UNTITLED_STB_STRING_H
#include <string.h>

typedef struct
{
    char* data;
    int length;
    int capacity;
} stb_string;

stb_string stb_make_string(char* data);
void stb_destroy_string(stb_string* s);
void stb_print_string(stb_string* s);
char* stb_string_at(stb_string* s, int index);
void stb_string_append(stb_string* s, char* data);
void stb_string_clear(stb_string* s);
void stb_string_reserve(stb_string* s, unsigned int capacity);
const char* stb_string_c_str(stb_string* s);
int stb_string_cmp(stb_string* s1, stb_string* s2);
void stb_string_insert(stb_string* s, int index, char* data);
void stb_string_pop(stb_string* s);
void stb_string_pop_at(stb_string* s, int index);
stb_string stb_string_copy(stb_string* s);
stb_string stb_string_substr(stb_string* s, int start, int length);
int stb_string_find(stb_string* s, char* data);
int stb_string_length(stb_string* s);
int stb_string_capacity(stb_string* s);
#define STB_STRING_NPOS (-1)

#ifdef STB_STRING_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>

stb_string stb_make_string(char* data)
{
    stb_string s;
    s.length = (int)strlen(data);
    s.capacity = s.length + 1; // +1 for null terminator
    s.data = malloc(s.capacity);
    if (s.data) {
        memcpy(s.data, data, s.capacity);
    }
    return s;
}

void stb_destroy_string(stb_string* s)
{
    free(s->data);
    s->data = NULL;
    s->length = 0;
    s->capacity = 0;
}

void stb_print_string(stb_string* s)
{
    printf("%s\n", s->data);
}

char* stb_string_at(stb_string* s, int index)
{
    if (index >= s->length || index < 0)
    {
        fprintf(stderr, "stb_string_at: index out of bounds at %d\n", index);
        return NULL;
    }
    return &s->data[index];
}

void stb_string_append(stb_string* s, char* data)
{
    if (s->length + strlen(data) + 1 > s->capacity)
    {
        char* new_data = realloc(s->data, (s->length + strlen(data) + 1) * 2);
        if (!new_data)
        {
            fprintf(stderr, "stb_string_append: realloc failed\n");
            return;
        }
        s->data = new_data;
        s->capacity = (s->length + strlen(data) + 1) * 2;
    }

    memcpy(s->data + s->length, data, strlen(data) + 1); // includes null terminator
    s->length += strlen(data);
}

void stb_string_clear(stb_string* s)
{
    s->length = 0;
    s->data[0] = '\0';
}

void stb_string_reserve(stb_string* s, unsigned int capacity)
{
    if (capacity <= (unsigned int)s->capacity)
    {
        return;
    }

    char* new_data = realloc(s->data, capacity);
    if (!new_data)
    {
        fprintf(stderr, "stb_string_reserve: realloc failed\n");
        return;
    }

    s->data = new_data;
    s->capacity = (int)capacity;
}

const char* stb_string_c_str(stb_string* s)
{
    return s->data;
}

int stb_string_cmp(stb_string* s1, stb_string* s2)
{
    return strcmp(s1->data, s2->data);
}

void stb_string_insert(stb_string* s, int index, char* data)
{
    if (index < 0 || index > s->length)
    {
        fprintf(stderr, "stb_string_insert: index out of bounds at %d\n", index);
        return;
    }

    int insert_len = (int)strlen(data);

    if (s->length + insert_len + 1 > s->capacity)
    {
        int new_capacity = (s->length + insert_len + 1) * 2;
        char* new_data = realloc(s->data, new_capacity);
        if (!new_data)
        {
            fprintf(stderr, "stb_string_insert: realloc failed\n");
            return;
        }
        s->data = new_data;
        s->capacity = new_capacity;
    }

    memmove(s->data + index + insert_len, s->data + index, s->length - index + 1);
    memcpy(s->data + index, data, insert_len);

    s->length += insert_len;
}

void stb_string_pop(stb_string* s)
{
    if (s->length == 0) return;
    s->length -= 1;
    s->data[s->length] = '\0';
}

void stb_string_pop_at(stb_string* s, int index)
{
    if (s->length == 0) return;
    if (index < 0 || index >= s->length)
    {
        fprintf(stderr, "stb_string_pop_at: index out of bounds at %d\n", index);
        return;
    }

    memmove(s->data + index, s->data + index + 1, s->length - index);

    s->length -= 1;
}

stb_string stb_string_copy(stb_string* s)
{
    stb_string str;
    str.length = (int)strlen(s->data);
    str.capacity = str.length + 1;
    str.data = malloc(str.capacity);
    return str;
}

stb_string stb_string_substr(stb_string* s, int start, int length)
{
    stb_string str;
    str.data = NULL;
    str.length = 0;
    str.capacity = 0;

    if (start < 0 || length < 0 || start + length > s->length)
    {
        fprintf(stderr, "stb_string_substr: range out of bounds (start=%d, length=%d)\n", start, length);
        return str;
    }

    str.capacity = length + 1; // +1 for null terminator
    str.data = malloc(str.capacity);
    if (!str.data)
    {
        fprintf(stderr, "stb_string_substr: malloc failed\n");
        str.capacity = 0;
        return str;
    }

    memcpy(str.data, s->data + start, length);
    str.data[length] = '\0';
    str.length = length;

    return str;
}

int stb_string_find(stb_string* s, char* find)
{
    int find_len = (int)strlen(find);

    if (find_len == 0) return 0;
    if (find_len > s->length) return STB_STRING_NPOS;

    for (int i = 0; i <= s->length - find_len; i++)
    {
        if (strncmp(s->data + i, find, find_len) == 0)
        {
            return i;
        }
    }

    return STB_STRING_NPOS;
}

#endif // STB_STRING_IMPLEMENTATION
#endif // UNTITLED_STB_STRING_H
