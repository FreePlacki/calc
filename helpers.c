#include "helpers.h"
#include <assert.h>
#include <string.h>

bool is_digit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

void reverse(char *str) {
    int length = strlen(str);
    for (int i = 0, j = length - 1; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

void parse_int(char *output, char *line) {
    int i = 0;
    while (line[i] != ' ' && line[i] != '\n') {
        i++;
    }
    strncpy(output, line, i);
    output[i] = '\0';

    line += i;
}

void read_arg(char *output, char *line) {
    parse_int(output, line);
    line++; // TODO: expect '\n'
}
