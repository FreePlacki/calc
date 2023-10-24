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

    char arg1[ARG_SIZE];
    char arg2[ARG_SIZE];
    dynStr result;
    init_dynStr(&result);

    oper op;
    bool ok = true;
    short ctr = 0;
    unsigned int line_idx = 0;
    char buffer[LINE_SIZE];
    while (fgets(buffer, sizeof(buffer), in_file) != NULL) {
        line_idx++;

        if (buffer[0] == '\n') {
            continue;
        }

        fprintf(out_file, "%s", buffer);

        if (!ok) {
            if (ctr != 0) {
                ctr = (ctr + 1) % 3;
                continue;
            }
            ok = true;
        }

        scanner scanner;
        scanner.idx = 0;
        scanner.line_idx = line_idx;
        scanner.line = buffer;

        // if (ctr == 0) {
        //     if (is_argument(buffer)) {
        //         // TODO
        //         // result.data = read_arg(&scanner, res, unsigned int base, bool *ok)
        //     }
        //     op = read_instruction(&scanner, &ok);
        //     if (op.op_type == Convert) {
        //     }
        // }

        if (ctr == 0) {
            op = read_instruction(&scanner, &ok);
            if (op.op_type == Convert) {
                // skip arg1
                ctr = 2;
                continue;
            }
        } else if (ctr == 1) {
            read_arg(&scanner, arg1, op.base, &ok);
            if (ok)
                consume(&scanner, '\n');
        } else if (ctr == 2) {
            read_arg(&scanner, arg2, op.base, &ok);
            if (!ok) {
                ctr = 0;
                continue;
            }

            dynStr output = execute(&scanner, op, arg1, arg2, &ok);
            if (scanner.line[strlen(scanner.line) - 1] != '\n') {
                fprintf(out_file, "\n");
            }
            if (ok)
                fprintf(out_file, "%s\n", output.data);

            free_dynStr(&output);
        }

        ctr = (ctr + 1) % 3;
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
