#ifndef SCANNER_H
#define SCANNER_H

#include "operation.h"

typedef struct {
    char *line;
    unsigned int line_idx;
    unsigned int idx;
} scanner;

void parse_int(scanner *scanner, char *output, unsigned int len);
oper read_instruction(scanner *scanner);
void read_arg(scanner *scanner, char *output);

char advance(scanner *scanner);
void consume(scanner *scanner, char c);

#endif
