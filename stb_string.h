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

#endif // STB_STRING_IMPLEMENTATION
#endif // UNTITLED_STB_STRING_H
