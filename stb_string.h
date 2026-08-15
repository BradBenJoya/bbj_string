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

#endif // STB_STRING_IMPLEMENTATION
#endif // UNTITLED_STB_STRING_H
