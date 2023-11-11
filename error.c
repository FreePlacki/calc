#include "error.h"
#include "helpers.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void report(scanner *scanner, er_type t, char *msg, ...) {
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

    if (scanner == NULL) {
        fprintf(stderr, ": ");
        vfprintf(stderr, msg, args);
        return;
    }

    fprintf(stderr, " [%d:%d]: ", scanner->line_idx, scanner->idx);
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\t%s", scanner->line);
    fprintf(stderr, "\t%*s^\n", scanner->idx > 0 ? scanner->idx-1 : scanner->idx, "");
}
