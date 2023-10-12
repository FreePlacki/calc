#include "helpers.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum op_type {
    Add,
    Mul,
    Div,
    Mod,
    Pow,
    Convert,
};

typedef struct {
    enum op_type op_type;
    unsigned short base;
} oper;

oper read_instruction(char *line) {
    oper op;

    while (*line != '\n') {
        char c = *line++;
        switch (c) {
        case '+':
            op.op_type = Add;
            line++; // TODO: expect ' '
            char base[2];
            parse_int(base, line);
            op.base = atoi(base);
            line++; // TODO: expect '\n'
            return op;
        case '*':
            op.op_type = Mul;
            break;
        case '/':
            op.op_type = Div;
            break;
        case '%':
            op.op_type = Mod;
            break;
        case '^':
            op.op_type = Pow;
            break;
        default:
            // kompilując w GCC można było użyć '0' ... '9'
            // ale nie chciałem wprowadzać rzeczy z poza standardu
            if (is_digit(c)) {
                op.op_type = Convert;
                break;
            }
            fprintf(stderr, "Nieoczekiwany znak `%c`\n", c);
        }
    }

    return op;
}

void exec_add(char *output, short base, char *arg1, char *arg2) {
    if (strlen(arg1) > strlen(arg2)) {
        char *t = arg1;
        arg1 = arg2;
        arg2 = t;
    }

    int n1 = strlen(arg1), n2 = strlen(arg2);

    reverse(arg1);
    reverse(arg2);

    int carry = 0;
    for (int i = 0; i < n1; i++) {
        int sum = ((arg1[i] - '0') + (arg2[i] - '0') + carry);
        char c = sum % 10 + '0';
        strncat(output, &c, 1);

        carry = sum / 10;
    }

    for (int i = n1; i < n2; i++) {
        int sum = ((arg2[i] - '0') + carry);
        char c = sum % 10 + '0';
        strncat(output, &c, 1);
        carry = sum / 10;
    }

    if (carry) {
        char c = carry + '0';
        strncat(output, &c, 1);
    }

    reverse(output);
}

void execute(char *output, oper op, char *arg1, char *arg2) {
    switch (op.op_type) {
    case Add:
        exec_add(output, op.base, arg1, arg2);
        break;
    }
    printf("OUTPUT: %s\n", output);
}

void print_help(char *name) {
    fprintf(stderr, "Uruchamianie:\n\t%s ", name);
    fprintf(
        stderr,
        "<ścieżka do pliku wejściowego> [ścieżka do pliku wyjściowego]\n\n");
    fprintf(stderr, "Plik wejściowy musi być w formacie `*.txt`\n");
    fprintf(stderr, "Plik wyjściowy jest opcjonalny ");
    fprintf(stderr, "(zostanie stworzony na podstawie nazwy pliku wejściowego "
                    "`out_*.txt`)\n");
}

int main(int argc, char **argv) {
    if (argc == 1 || argc > 3) {
        print_help(argv[0]);
        exit(1);
    }

    FILE *in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        fprintf(stderr, "Nie można otworzyć pliku `%s`\n", argv[1]);
        exit(1);
    }
    // TODO
    FILE *out_file = fopen(argc == 3 ? argv[2] : "out_.txt", "w");

    oper op;
    char arg1[40];
    char arg2[40];
    int ctr = 0;
    // Liczby mają długość 40 więc powinniśmy zmieścić się w 100 znakach na
    // linię
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), in_file) != NULL) {
        if (buffer[0] == '\n') {
            continue;
        }

        fprintf(out_file, "%s", buffer);

        if (ctr == 0) {
            op = read_instruction(buffer);
        } else if (ctr == 1) {
            read_arg(arg1, buffer);
        } else if (ctr == 2) {
            read_arg(arg2, buffer);
            char *output = (char *)malloc(sizeof(char[100]));
            output[0] = '\0';
            execute(output, op, arg1, arg2);
            fprintf(out_file, "%s\n", output);
            free(output);
            ctr = -1;
        }
        ctr++;
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
