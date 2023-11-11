#include "error.h"
#include "execute.h"
#include "helpers.h"
#include "operation.h"
#include "scanner.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

void dump_result(FILE *out_file, dynStr *result, int arg_count,
                 int expected_count, int line_idx, char *op_buffer,
                 bool is_repl, bool ok) {
    if (!ok) {
        free_dynStr(result);
        return;
    }
    if (arg_count < expected_count) {
        // towrzymy scanner z nagłówkiem błędnej operacji
        scanner scanner;
        scanner.line_idx = line_idx;
        scanner.line = op_buffer;
        scanner.idx = 1;
        report(&scanner, warning,
               "Operacja przyjmuje przynajmniej %s, "
               "otrzymano "
               "%d\n",
               expected_count == 1 ? "1 argument" : "2 argumenty", arg_count);
    }

    if (result->data) {
        if (is_repl) {
            printf("Wynik: ");
        }
        fprintf(out_file, "%s\n\n", result->data);
    }
    free_dynStr(result);
}

void repl_prompt(int line_idx, bool is_repl) {
    if (is_repl)
        printf("[%d]> ", line_idx + 1);
}

int main(int argc, char **argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // poprawne kodowanie na windowsie
#endif

    if (argc > 3) {
        print_help(argv[0]);
        exit(1);
    }

    FILE *in_file;
    FILE *out_file;
    bool is_repl = false;
    if (argc == 1) {
        in_file = stdin;
        out_file = stdout;
        is_repl = true;
    } else {
        in_file = fopen(argv[1], "r");
        if (in_file == NULL) {
            report(NULL, error, "Nie można otworzyć pliku `%s`\n", argv[1]);
            exit(1);
        }

        char *out_name;
        if (argc == 3) {
            out_name = argv[2];
        } else {
            char o_name[100] = "out_";
            out_name = strcat(o_name, extract_name(argv[1]));
            out_name = strcat(out_name, ".txt");
            report(NULL, info, "Nie podano pliku wyjściowego, stworzono `%s`\n",
                   out_name);
        }
        out_file = fopen(out_name, "w");
    }

    dynStr result;
    oper op;
    bool ok = true;
    unsigned int line_idx = 0;
    char buffer[LINE_SIZE];

    // do raportowania błędów
    char op_buffer[LINE_SIZE];
    int last_op_idx = 0;
    int arg_count = 0;
    int expected_args = 2;

    if (is_repl)
        printf("%s repl\nAby zakończyć: ctrl-d (ctrl-z na windowsie)\n\n",
               argv[0]);

    while (repl_prompt(line_idx, is_repl),
           fgets(buffer, sizeof(buffer), in_file) != NULL) {
        line_idx++;

        if (buffer[0] == '\n' || buffer[0] == '#') {
            continue;
        }

        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        scanner scanner;
        scanner.idx = 0;
        scanner.line_idx = line_idx;
        scanner.line = buffer;

        if (!ok) {
            ok = true;
            if (arg_count >= expected_args)
                dump_result(out_file, &result, arg_count, expected_args,
                            line_idx, op_buffer, is_repl, ok);
            // jeżeli nie zaczyna się jeszcze nowe działanie lub przy
            // parsowaniu wystąpił błąd to nadal pomijamy tą linię
            if (is_argument(&scanner, buffer, &ok) || !ok) {
                ok = false;
                continue;
            }
        }

        if (is_argument(&scanner, buffer, &ok)) {
            if (!ok)
                continue;

            if (last_op_idx == 0) {
                report(&scanner, error, "Nie podano nazwy operacji\n");
                ok = false;
                continue;
            }

            int base = op.op_type == Con ? (op.base >> 4) + 2 : op.base;

            char arg[ARG_SIZE];
            read_arg(&scanner, arg, base, &ok);
            arg_count++;

            if (!ok)
                continue;

            if (arg_count >= expected_args) {
                result = execute(&scanner, op, result.data, arg, &ok);
                if (!ok)
                    continue;

                if (op.op_type == Con) {
                    if (!is_repl)
                        fprintf(out_file, "%s\n\n", arg);
                    dump_result(out_file, &result, arg_count, expected_args,
                                last_op_idx, op_buffer, is_repl, ok);
                    continue;
                }
            } else {
                dynStr_from(&result, arg);
            }
        } else {
            if (last_op_idx != 0 && line_idx != 1)
                dump_result(out_file, &result, arg_count, expected_args,
                            last_op_idx, op_buffer, is_repl, ok);

            op = read_instruction(&scanner, &ok);
            if (!ok)
                continue;

            expected_args = op.op_type == Con ? 1 : 2;
            arg_count = 0;
            strcpy(op_buffer, buffer);
            last_op_idx = line_idx;
        }

        if (!is_repl)
            fprintf(out_file, "%s\n", buffer);
    }

    dump_result(out_file, &result, arg_count, expected_args, last_op_idx,
                op_buffer, is_repl, ok);

    fclose(in_file);
    fclose(out_file);
    return 0;
}
