#ifndef ERROR_H
#define ERROR_H
#include "scanner.h"

typedef enum {
    error,
    warning,
    info,
} er_type;

void report(scanner *scanner, er_type t, char *msg, ...);

#endif
