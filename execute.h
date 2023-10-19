#ifndef EXECUTE_H
#define EXECUTE_H

#include "helpers.h"

dynStr execute(scanner *scanner, oper op, char *arg1, char *arg2, bool *ok);

#endif
