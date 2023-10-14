#include "error.h"
#include "helpers.h"
#include "operation.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 255

dynStr exec_add(scanner *scanner, short base, char *arg1, char *arg2) {
    if (strlen(arg1) > strlen(arg2)) {
        char *t = arg1;
        arg1 = arg2;
        arg2 = t;
    }

    int n1 = strlen(arg1), n2 = strlen(arg2);

    reverse(arg1);
    reverse(arg2);

    dynStr output;
    init_dynStr(&output);

    int carry = 0;
    for (int i = 0; i < n2; i++) {
        int digit1 = i < n1 ? char_to_dec(scanner, arg1[i], base) : 0;
        int digit2 = char_to_dec(scanner, arg2[i], base);
        int sum = digit1 + digit2 + carry;

        char c = int_to_char(sum % base);
        append_char(&output, c);

        carry = sum / base;
    }

    if (carry) {
        char c = int_to_char(carry);
        append_char(&output, c);
    }

    reverse(output.data);
    return output;
}

dynStr execute(scanner *scanner, oper op, char *arg1, char *arg2) {
    switch (op.op_type) {
    case Add:
        return exec_add(scanner, op.base, arg1, arg2);
        break;
    }
}

void print_help(char *name) {
    fprintf(stderr, "Użycie:\n\t%s ", name);
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

    char *out_name;
    if (argc == 3) {
        out_name = argv[2];
    } else {
        char o_name[100] = "out_";
        out_name = strcat(o_name, extract_name(argv[1]));
        out_name = strcat(out_name, ".txt");
        report(NULL, info, "Nie podano pliku wyjściowego, stworzono `%s`\n", out_name);
    }
    FILE *out_file = fopen(out_name, "w");

    oper op;
    char arg1[40];
    char arg2[40];
    short ctr = 0;
    char buffer[LINE_SIZE];
    unsigned int line_idx = 0;
    while (fgets(buffer, sizeof(buffer), in_file) != NULL) {
        line_idx++;

        if (buffer[0] == '\n') {
            continue;
        }

        fprintf(out_file, "%s", buffer);

        scanner scanner;
        scanner.idx = 0;
        scanner.line_idx = line_idx;
        scanner.line = buffer;

        if (ctr == 0) {
            op = read_instruction(&scanner);
        } else if (ctr == 1) {
            read_arg(&scanner, arg1);
        } else if (ctr == 2) {
            read_arg(&scanner, arg2);
            dynStr output = execute(&scanner, op, arg1, arg2);
            fprintf(out_file, "%s\n", output.data);

            free_dynStr(&output);
            ctr = -1;
        }
        ctr++;
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
