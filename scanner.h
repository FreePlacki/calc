#ifndef SCANNER_H
#define SCANNER_H

#include "operation.h"
#include <stdbool.h>

#define ARG_SIZE 41
#define LINE_SIZE 256

typedef struct {
    char *line;
    unsigned int line_idx;
    unsigned int idx;
} scanner;

void parse_int(scanner *scanner, char *output, unsigned int len,
               unsigned int base, bool *ok);
oper read_instruction(scanner *scanner, bool *ok);
void read_arg(scanner *scanner, char *output, unsigned int base, bool *ok);
bool is_argument(scanner *scanner, char *line);

char advance(scanner *scanner);
void consume(scanner *scanner, char c);
void consume_spaces(scanner *scanner);

#endif
