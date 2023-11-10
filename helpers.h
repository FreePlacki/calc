#ifndef HELPERS_H
#define HELPERS_H

#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>

#define dbg(x, y) fprintf(stderr, "%d:" #y " = " x "\n", __LINE__, y);

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} dynStr;

void init_dynStr(dynStr *str);
void dynStr_from(dynStr *str, char *s);
void append_char(dynStr *str, char c);
void free_dynStr(dynStr *str);

void print_help(char *name);
void reverse(char *str);
bool is_digit(char c, unsigned int base);
bool is_unknown_char(scanner *scanner, char c);
int char_to_dec(char c, short base, bool *ok);
char int_to_char(int v);
void trim_trailing(dynStr *num, char c);
void trim_leading(dynStr *num, char c);
const char *extract_name(const char *path);

#endif
