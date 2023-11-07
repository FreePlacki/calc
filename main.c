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

void dump_result(FILE *out_file, dynStr *result) {
    fprintf(out_file, "%s\n\n", result->data);
    free_dynStr(result);
}

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
    unsigned int line_idx = 0;
    char buffer[LINE_SIZE];
    int arg_count = 0;
    while (fgets(buffer, sizeof(buffer), in_file) != NULL) {
        line_idx++;

        if (buffer[0] == '\n' || buffer[0] == '#') {
            continue;
        }

        if (!ok) {
            if (!is_argument(buffer)) {
                arg_count = 0;
                ok = true;
            } else {
                continue;
            }
        }

        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        scanner scanner;
        scanner.idx = 0;
        scanner.line_idx = line_idx;
        scanner.line = buffer;

        char arg[ARG_SIZE];

        if (is_argument(buffer)) {
            int base = op.op_type == Convert ? (op.base >> 4) + 2 : op.base;
            read_arg(&scanner, arg, base, &ok);
            if (arg_count > 0 || op.op_type == Convert) {
                result = execute(&scanner, op, result.data, arg, &ok);
                if (op.op_type == Convert) {
                    fprintf(out_file, "%s\n\n", arg);
                    dump_result(out_file, &result);
                    continue;
                }
            } else {
                dynStr_from(&result, arg);
            }
            arg_count++;
        } else {
            if (arg_count > 1) {
                dump_result(out_file, &result);
            } else {
                // nie dajemy scannera bo musiałby się odnosić do buffera z
                // poprzednich pętli
                if (line_idx != 1 && op.op_type != Convert)
                    report(NULL, warning,
                           "Operacja przyjmuje przynajmniej 2 argumenty, "
                           "otrzymano "
                           "%d\n",
                           arg_count);
            }
            op = read_instruction(&scanner, &ok);
            arg_count = 0;
        }

        fprintf(out_file, "%s\n", buffer);
    }

    if (result.data)
        dump_result(out_file, &result);


    fclose(in_file);
    fclose(out_file);
    return 0;
}
