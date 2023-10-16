#include "scanner.h"
#include "error.h"
#include "execute.h"
#include "helpers.h"
#include "operation.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_SIZE 40

void parse_int(scanner *scanner, char *output, unsigned int len,
               unsigned int base, bool *ok) {
    char *start = scanner->line + scanner->idx;
    // pewnie dało by się bez wprowadzania 'i' ale czytelność by się pogorszyła
    unsigned int i = 0;
    while (start[i] != '\0' && start[i] != '\n' && start[i] != ' ') {
        char c = start[i++];
        scanner->idx++;

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
    }
    strncpy(output, start, i);
    output[i] = '\0';
}

unsigned short parse_base(scanner *scanner, bool *ok) {
    char base[2];
    parse_int(scanner, base, 2, 10, ok);
    unsigned short res = atoi(base);
    if (res < 2 || res > 16) {
        // Jeżeli nie udało się odczytać bazy to nie wypisujmy drugiego błędu
        if (*ok)
            report(scanner, error,
                   "Baza może być tylko z przedziału [2, 16]\n");
        if (ok)
            *ok = false;
        return res;
    }
    return res;
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
    // TODO: implement current(scanner)
    while (scanner->line[scanner->idx] == ' ') {
        advance(scanner);
    }
}

void read_arg(scanner *scanner, char *output, unsigned int base, bool *ok) {
    parse_int(scanner, output, ARG_SIZE, base, ok);
}

oper read_instruction(scanner *scanner, bool *ok) {
    oper op;
    char c = advance(scanner);

    if (is_digit(c, 10)) {
        scanner->idx--;

        unsigned short base1 = parse_base(scanner, ok);
        if (!(*ok)) return op;

        consume_spaces(scanner);
        
        unsigned short base2 = parse_base(scanner, ok);
        if (!(*ok)) return op;

        op.base = (base1 << 4) | base2;
        op.op_type = Convert;
        return op;
    }

    consume_spaces(scanner);

    op.base = parse_base(scanner, ok);
    if (!(*ok)) return op;

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
        if (ok)
            *ok = false;
    }

    return op;
}
