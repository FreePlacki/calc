#include "error.h"
#include "helpers.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    error,
    warning,
    info,
} er_type;

void report(unsigned int line_idx, unsigned int char_idx, er_type t, char *msg,
            ...) {
    // https://stackoverflow.com/questions/68154231/how-do-i-define-a-function-that-accepts-a-formatted-input-string-in-c

    va_list args;
    va_start(args, msg);
    switch (t) {
    case error:
        fprintf(stderr, "\x1b[31mERROR\x1b[0m");
        break;
    case warning:
        fprintf(stderr, "\x1b[33mWARNING\x1b[0m");
        break;
    case info:
        fprintf(stderr, "\x1b[34mINFO\x1b[0m");
        break;
    }
    fprintf(stderr, " [%d:%d]: ", line_idx, char_idx);
    vfprintf(stderr, msg, args);
}

void consume(char c, char *line, unsigned int line_idx,
             unsigned int *char_idx) {
    if (line[*char_idx] != c) {
        report(line_idx, *char_idx, error, "Expected `%c`\n", c);
        exit(1);
    }
    (*char_idx)++;
}
