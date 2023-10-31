#include "execute.h"
#include "error.h"
#include "helpers.h"
#include "scanner.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dynStr exec_add(short base, char *arg1, char *arg2) {
    dynStr result;
    init_dynStr(&result);

    int n1 = strlen(arg1), n2 = strlen(arg2);

    int carry = 0;
    int i, j;
    for (i = n1 - 1, j = n2 - 1; i >= 0; i--, j--) {
        // Możemy (chyba) dać NULL za ok, bo już sprawdziliśmy błędny input
        int digit1 = (i >= 0) ? char_to_dec(arg1[i], base, NULL) : 0;
        int digit2 = (j >= 0) ? char_to_dec(arg2[j], base, NULL) : 0;
        int sum = digit1 + digit2 + carry;

        char c = int_to_char(sum % base);
        append_char(&result, c);

        carry = sum / base;
    }

    if (carry) {
        char c = int_to_char(carry);
        append_char(&result, c);
    }

    reverse(result.data);
    return result;
}

dynStr exec_sub(short base, char *arg1, char *arg2) {
    dynStr result;
    init_dynStr(&result);

    int n1 = strlen(arg1), n2 = strlen(arg2);

    if (n1 < n2) {
        append_char(&result, '0');
        return result;
    }

    int carry = 0;
    int i, j;
    for (i = n1 - 1, j = n2 - 1; i >= 0; i--, j--) {
        int digit1 = char_to_dec(arg1[i], base, NULL);
        int digit2 = j >= 0 ? char_to_dec(arg2[j], base, NULL) : 0;
        int diff = digit1 - digit2 - carry;

        if (diff < 0) {
            diff += base;
            carry = 1;
        } else {
            carry = 0;
        }

        char c = int_to_char(diff);
        append_char(&result, c);
    }

    trim_trailing(&result, '0');
    reverse(result.data);

    return result;
}

dynStr exec_mul(short base, char *arg1, char *arg2) {
    int n1 = strlen(arg1), n2 = strlen(arg2);

    dynStr result;
    init_dynStr(&result);
    for (int i = 0; i < n1 + n2; i++) {
        append_char(&result, '0');
    }

    for (int i = n1 - 1; i >= 0; i--) {
        int carry = 0;
        int digit1 = char_to_dec(arg1[i], base, NULL);

        for (int j = n2 - 1; j >= 0; j--) {
            int digit2 = char_to_dec(arg2[j], base, NULL);
            int product =
                digit1 * digit2 + carry +
                char_to_dec(result.data[n1 + n2 - (i + j) - 2], base, NULL);

            carry = product / base;
            result.data[n1 + n2 - (i + j) - 2] = int_to_char(product % base);
        }

        if (carry > 0) {
            result.data[n1 - i + n2 - 1] = int_to_char(
                char_to_dec(result.data[n1 - i + n2 - 1], base, NULL) + carry);
        }
    }

    trim_trailing(&result, '0');
    reverse(result.data);

    return result;
}

int compare(char *arg1, char *arg2) {
    int len1 = strlen(arg1);
    int len2 = strlen(arg2);

    if (len1 > len2) {
        return 1;
    } else if (len1 < len2) {
        return -1;
    }
    return strcmp(arg1, arg2);
}

dynStr exec_div(scanner *scanner, short base, char *arg1, char *arg2, char *m,
                bool *ok) {
    dynStr result;
    init_dynStr(&result);
    append_char(&result, '0');

    if (arg2[0] == '0') {
        report(scanner, error, "Division by 0!\n");
        if (ok)
            *ok = false;
        return result;
    }

    dynStr remaining;
    init_dynStr(&remaining); // have to init if res = 0

    int index = 0;
    while (arg1[index] != '\0') {
        append_char(&remaining, arg1[index]);

        int quotient = 0;
        while (compare(remaining.data, arg2) >= 0) {
            remaining = exec_sub(base, remaining.data, arg2);
            quotient++;
        }
        // reset remaining size
        if (quotient) {
            dynStr_from(&remaining, remaining.data);
        }

        char c = int_to_char(quotient);
        append_char(&result, c);
        index++;
    }

    if (m) {
        strcpy(m, remaining.data);
    }

    free_dynStr(&remaining);
    trim_leading(&result, '0');

    return result;
}

dynStr exec_mod(scanner *scanner, short base, char *arg1, char *arg2,
                bool *ok) {
    char mod[40];
    exec_div(scanner, base, arg1, arg2, mod, ok);

    dynStr result;
    if (*ok) {
        dynStr_from(&result, mod);
    }

    return result;
}

dynStr to_dec(short from, char *arg) {
    dynStr result;
    init_dynStr(&result);
    append_char(&result, '0');

    char from_s[8];
    sprintf(from_s, "%d", from);
    int len = strlen(arg);
    for (int i = 0; i < len; i++) {
        int val = char_to_dec(arg[i], from, NULL);
        char val_s[8];
        sprintf(val_s, "%d", val);

        result = exec_add(10, result.data, val_s);
        if (i != len - 1)
            result = exec_mul(10, result.data, from_s);
    }

    return result;
}

dynStr exec_convert(scanner *scanner, short base, char *arg) {
    short from = (base >> 4) + 2;
    short to = (base & 0xF) + 2;

    dynStr div = to_dec(from, arg);
    dynStr result;
    init_dynStr(&result);

    while (true) {
        char mod[1];
        char to_str[4];
        sprintf(to_str, "%d", to);
        div = exec_div(scanner, 10, div.data, to_str, mod, NULL);
        append_char(&result, mod[0]);
        if (div.data[0] == '0')
            break;
    }

    free_dynStr(&div);
    reverse(result.data);
    return result;
}

dynStr exec_pow(short base, char *arg1, char *arg2) {
    dynStr result;
    init_dynStr(&result);
    append_char(&result, '1');

    unsigned long exp = atol(to_dec(base, arg2).data);
    if (atoi(to_dec(base, arg1).data) == 1 || exp == 0) {
        return result;
    }

    // https://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
    // 2 ^ FFFFF zajmuje ~2min
    while (true) {
        if (exp & 1)
            result = exec_mul(10, result.data, arg1);
        exp >>= 1;
        if (!exp)
            break;
        arg1 = exec_mul(base, arg1, arg1).data;
    }

    return result;
}

dynStr execute(scanner *scanner, oper op, char *arg1, char *arg2, bool *ok) {
    switch (op.op_type) {
    case Add:
        return exec_add(op.base, arg1, arg2);
    case Mul:
        return exec_mul(op.base, arg1, arg2);
    case Pow:
        return exec_pow(op.base, arg1, arg2);
    case Div:
        return exec_div(scanner, op.base, arg1, arg2, NULL, ok);
    case Mod:
        return exec_mod(scanner, op.base, arg1, arg2, ok);
    case Convert:
        return exec_convert(scanner, op.base, arg2);
    default:
        assert("Unreachable" && 0);
    }
}
