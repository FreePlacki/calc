#ifndef EXECUTE_H
#define EXECUTE_H

#include "helpers.h"

dynStr exec_add(scanner *scanner, short base, char *arg1, char *arg2);
dynStr exec_mul(scanner *scanner, short base, char *arg1, char *arg2);
dynStr exec_pow(scanner *scanner, short base, char *arg1, char *arg2);
dynStr execute(scanner *scanner, oper op, char *arg1, char *arg2, bool *ok);

#endif
