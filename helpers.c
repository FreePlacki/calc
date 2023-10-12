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

unsigned int parse_int(char *output, char *line, unsigned int idx) {
    int i = 0;
    while (line[i+idx] != ' ' && line[i+idx] != '\n') {
        i++;
    }
    strncpy(output, line + idx, i);
    output[i] = '\0';

    return i;
}

void read_arg(char *output, char *line) {
    parse_int(output, line, 0);
    line++; // TODO: expect '\n'
}
