#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include <stdio.h>

#define dbg(x, y) fprintf(stderr, "%d:" #y " = " x "\n", __LINE__, y);

void reverse(char *str);
void parse_int(char *output, char *line);
bool is_digit(char c);
void append_chr(char *str, char c);
void read_arg(char *output, char *line);

#endif
