#include "helpers.h"
#include "error.h"
#include "scanner.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void init_dynStr(dynStr *str) {
    str->size = 0;
    str->capacity = 8;
    str->data = (char *)malloc(sizeof(char) * str->capacity);
    str->data[0] = '\0';
}

void append_char(dynStr *str, char c) {
    if (str->size + 1 >= str->capacity) {
        str->capacity *= 2;
        str->data = (char *)realloc(str->data, str->capacity);
    }

    str->data[str->size] = c;
    str->size++;
    str->data[str->size] = '\0';
}

void free_dynStr(dynStr *str) {
    free(str->data);
    str->data = NULL;
    str->size = 0;
    str->capacity = 0;
}

bool is_digit(char c, unsigned int base) {
    bool ok = true;
    char_to_dec(NULL, c, base, &ok);
    return ok;
}

void reverse(char *str) {
    int length = strlen(str);
    for (int i = 0, j = length - 1; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int char_to_dec(scanner *scanner, char c, unsigned int base, bool *ok) {
    int res;
    if (c >= '0' && c <= '9') {
        res = c - '0';
    } else if (c >= 'A' && c <= 'F') {
        res = 10 + c - 'A';
    } else if (c >= 'a' && c <= 'f') {
        res = 10 + c - 'a';
    } else {
        res = -1;
    }

    if (res == -1 || res > base - 1) {
        if (ok)
            *ok = false;
    }

    return res;
}

char int_to_char(int v) {
    assert(v >= 0 && v <= 15);

    if (v >= 0 && v <= 9) {
        return '0' + v;
    }
    return 'A' + v - 10;
}

const char *extract_name(const char *path) {
    const char *filename;

    char slash = '/';
#ifdef _WIN32
    slash = '\\';
#endif
    const char *last_slash = strrchr(path, slash);

    if (last_slash != NULL) {
        filename = last_slash + 1;
    } else {
        filename = path;
    }

    const char *last_dot = strrchr(filename, '.');

    if (last_dot != NULL) {
        size_t len = last_dot - filename;
        char *new_str = (char *)malloc(sizeof(char) * (len + 1));
        strncpy(new_str, filename, len);
        new_str[len] = '\0';
        return strdup(new_str);
    } else {
        return strdup(filename);
    }
}
