#include "error.h"
#include "execute.h"
#include "helpers.h"
#include "operation.h"
#include "scanner.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char **argv) {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // poprawne kodowanie na windowsie
#endif

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
        report(NULL, info, "Nie podano pliku wyjściowego, stworzono `%s`\n",
               out_name);
    }
    FILE *out_file = fopen(out_name, "w");

    dynStr result;

    oper op;
    bool ok = true;
    bool has_args = false;
    unsigned int line_idx = 0;
    char buffer[LINE_SIZE];
    while (fgets(buffer, sizeof(buffer), in_file) != NULL) {
        line_idx++;

        if (buffer[0] == '\n') {
            continue;
        }

        if (!ok) {
            if (!is_argument(buffer)) {
                has_args = false;
                ok = true;
            } else {
                continue;
            }
        }

        scanner scanner;
        scanner.idx = 0;
        scanner.line_idx = line_idx;
        scanner.line = buffer;

        char arg[ARG_SIZE];

        if (is_argument(buffer)) {
            read_arg(&scanner, arg, op.base, &ok);
            if (has_args) {
                result = execute(&scanner, op, result.data, arg, &ok);
            } else {
                dynStr_from(&result, arg);
            }
            has_args = true;
        } else {
            if (has_args) {
                fprintf(out_file, "%s\n", result.data);
                free_dynStr(&result);
            }
            op = read_instruction(&scanner, &ok);
            has_args = op.op_type == Convert;
        }

        fprintf(out_file, "%s", buffer);
    }

    if (result.data) {
        fprintf(out_file, "%s\n", result.data);
        free_dynStr(&result);
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
