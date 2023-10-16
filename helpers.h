#ifndef HELPERS_H
#define HELPERS_H

#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>

#define dbg(x, y) fprintf(stderr, "%d:" #y " = " x "\n", __LINE__, y);

typedef struct {
    char *data;
    unsigned int size;
    unsigned int capacity;
} dynStr;

void init_dynStr(dynStr *str);
void append_char(dynStr *str, char c);
void free_dynStr(dynStr *str);

void print_help(char *name);
void reverse(char *str);
bool is_digit(char c, unsigned int base);
int char_to_dec(scanner *scanner, char c, unsigned int base, bool *ok);
char int_to_char(int v);
const char *extract_name(const char *path);

#endif
