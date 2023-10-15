#include "scanner.h"
#include "error.h"
#include "execute.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_SIZE 40

void parse_int(scanner *scanner, char *output, unsigned int len) {
    char *start = scanner->line + scanner->idx;
    // pewnie dało by się bez wprowadzania 'i' ale czytelność by się pogorszyła
    unsigned int i = 0;
    while (start[i] != ' ' && start[i] != '\n') {
        i++;
        scanner->idx++;
        if (i > len) {
            report(scanner, error,
                   "Długość argumentu nie może przekraczać %d\n", len);
            exit(1);
        }
    }
    strncpy(output, start, i);
    output[i] = '\0';
}

void read_arg(scanner *scanner, char *output) {
    parse_int(scanner, output, ARG_SIZE);
    consume(scanner, '\n');
}

void consume(scanner *scanner, char c) {
    if (scanner->line[scanner->idx] != c) {
        report(scanner, error,
               c == '\n' ? "Oczekiwano `\\n`\n" : "Oczekiwano `%c`\n", c);
        exit(1);
    }
    scanner->idx++;
}

oper read_instruction(scanner *scanner) {
    oper op;

    // TODO: add scanned advance which returns line[idx]
    char c = scanner->line[scanner->idx];
    scanner->idx++;

    if (is_digit(c)) {
        op.op_type = Convert;
        return op;
    }

    consume(scanner, ' ');

    char base[2];
    parse_int(scanner, base, 2);
    op.base = atoi(base);
    if (op.base < 2 || op.base > 16) {
        if (op.base == 0) {
            report(scanner, warning, "Nie podano bazy, przyjęto 10\n");
            return op;
        }
        report(scanner, error, "Baza może być tylko z przedziału [2, 16]\n");
        // TODO: instead of exit skip the next arguemnts and execute the
        // rest of the file
        exit(1);
    }

    consume(scanner, '\n');

    switch (c) {
    case '+':
        op.op_type = Add;
        break;
    case '*':
        op.op_type = Mul;
        break;
    case '^':
        op.op_type = Pow;
        break;
    case '/':
        op.op_type = Div;
        report(scanner, error, "Działanie jeszcze nie zaimplementowane\n");
        exit(1);
        break;
    case '%':
        op.op_type = Mod;
        report(scanner, error, "Działanie jeszcze nie zaimplementowane\n");
        exit(1);
        break;
    default:
        report(scanner, error, "Nieoczekiwany znak `%c`\n", c);
    }

    return op;
}
