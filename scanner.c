#include "scanner.h"
#include "error.h"
#include "execute.h"
#include "helpers.h"
#include "operation.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_int(scanner *scanner, char *output, unsigned int len,
               unsigned int base, bool *ok) {
    char *start = scanner->line + scanner->idx;
    unsigned int i = 0;

    // ucinamy 0 poprzedzające liczbę (np. 002 -> 2, 00 -> 0)
    while (start[0] == '0' && start[2] != '\0') {
        start++;
    }

    while (start[i] != '\0' && start[i] != '\n' && start[i] != '\r' &&
           start[i] != ' ') {
        char c = toupper(start[i++]);
        scanner->idx++;

        if (is_unknown_char(scanner, c)) {
            if (ok)
                *ok = false;
            return;
        }

        if (!is_digit(c, base)) {
            report(scanner, error, "Nie można użyć `%c` w liczbie o bazie %d\n",
                   c, base);
            if (ok)
                *ok = false;
            return;
        }

        if (i > len) {
            report(scanner, error,
                   "Długość argumentu nie może przekraczać %d\n", len);
            if (ok)
                *ok = false;
            return;
        }

        output[i - 1] = c;
    }

    output[i] = '\0';
}

unsigned short parse_base(scanner *scanner, bool *ok) {
    char base[3];
    parse_int(scanner, base, 2, 10, ok);
    if (!*ok)
        return 0;
    unsigned short res = atoi(base);
    if (res < 2 || res > 16) {
        report(scanner, error, "Baza może być tylko z przedziału [2, 16]\n");
        if (ok)
            *ok = false;
        return res;
    }
    return res;
}

bool is_argument(scanner *scanner, char *line) {
    // traktujemy nieznane znaki jako argument aby potem
    // w parsowaniu wyrzucić błąd
    if (is_unknown_char(scanner, line[0]))
        return true;

    int i = 0;
    while (line[i] != '\n' && line[i] != '\r' && line[i] != '\0') {
        if (!is_digit(line[i], 16))
            return false;
        i++;
    }

    return true;
}

char advance(scanner *scanner) { return scanner->line[scanner->idx++]; }

void consume(scanner *scanner, char c) {
    if (advance(scanner) != c) {
        report(scanner, error,
               c == '\n' ? "Oczekiwano `\\n`\n" : "Oczekiwano `%c`\n", c);
        exit(1);
    }
}

void consume_spaces(scanner *scanner) {
    while (scanner->line[scanner->idx] == ' ') {
        advance(scanner);
    }
}

void read_arg(scanner *scanner, char *output, unsigned int base, bool *ok) {
    parse_int(scanner, output, ARG_SIZE - 1, base, ok);
}

oper read_instruction(scanner *scanner, bool *ok) {
    oper op;
    op.op_type = Add;
    op.base = 0;
    char c = advance(scanner);

    if (is_digit(c, 10)) {
        scanner->idx--;

        unsigned short base1 = parse_base(scanner, ok);
        if (!(*ok))
            return op;

        consume_spaces(scanner);

        unsigned short base2 = parse_base(scanner, ok);
        if (!(*ok))
            return op;

        op.base = ((base1 - 2) << 4) | (base2 - 2);
        op.op_type = Convert;
        return op;
    }

    consume_spaces(scanner);

    op.base = parse_base(scanner, ok);
    if (!(*ok))
        return op;

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
        break;
    case '%':
        op.op_type = Mod;
        break;
    default:
        report(scanner, error, "Nieoczekiwany znak `%c`\n", c);
        if (ok)
            *ok = false;
    }

    return op;
}
