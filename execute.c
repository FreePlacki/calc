#include "execute.h"
#include "error.h"
#include "helpers.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // Możemy (chyba) dać NULL za ok, bo już sprawdziliśmy błędny input
        int digit1 = i < n1 ? char_to_dec(scanner, arg1[i], base, NULL) : 0;
        int digit2 = char_to_dec(scanner, arg2[i], base, NULL);
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

dynStr exec_sub(scanner *scanner, short base, char *arg1, char *arg2) {
    dynStr result;
    init_dynStr(&result);

    int n1 = strlen(arg1);
    int n2 = strlen(arg2);

    if (n1 < n2) {
        append_char(&result, '0');
        return result;
    }

    int borrow = 0;
    int i, j;
    for (i = n1 - 1, j = n2 - 1; i >= 0; i--, j--) {
        int digit1 = char_to_dec(scanner, arg1[i], base, NULL);
        int digit2 = j >= 0 ? char_to_dec(scanner, arg2[j], base, NULL) : 0;
        int diff = digit1 - digit2 - borrow;

        if (diff < 0) {
            diff += base;
            borrow = 1;
        } else {
            borrow = 0;
        }

        char c = int_to_char(diff);
        append_char(&result, c);
    }

    // Remove leading zeros
    for (int i = result.size - 1; i > 0; i--) {
        if (result.data[i] == '0') {
            result.data[i] = '\0';
        } else {
            break;
        }
    }
    reverse(result.data);

    return result;
}

dynStr exec_mul(scanner *scanner, short base, char *arg1, char *arg2) {
    int n1 = strlen(arg1);
    int n2 = strlen(arg2);

    // TODO: get rid of reverse
    reverse(arg1);
    reverse(arg2);

    dynStr result;
    init_dynStr(&result);
    for (int i = 0; i < n1 + n2; i++) {
        append_char(&result, '0');
    }

    for (int i = 0; i < n1; i++) {
        int carry = 0;
        int digit1 = char_to_dec(scanner, arg1[i], base, NULL);

        for (int j = 0; j < n2; j++) {
            int digit2 = char_to_dec(scanner, arg2[j], base, NULL);
            int product = digit1 * digit2 + carry +
                          char_to_dec(scanner, result.data[i + j], base, NULL);

            carry = product / base;
            result.data[i + j] = int_to_char(product % base);
        }

        if (carry > 0) {
            result.data[i + n2] = int_to_char(
                char_to_dec(scanner, result.data[i + n2], base, NULL) + carry);
        }
    }

    // Remove leading zeros
    for (int i = result.size - 1; i > 0; i--) {
        if (result.data[i] == '0') {
            result.data[i] = '\0';
        } else {
            break;
        }
    }

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
        // TODO: hande a case like arg2 = '01'
        report(scanner, error, "Division by 0!\n");
        if (ok)
            *ok = false;
        return result;
    }

    dynStr remaining;
    while (compare(arg1, arg2) >= 0) {
        remaining = exec_sub(scanner, base, arg1, arg2);
        arg1 = remaining.data;
        result = exec_add(scanner, base, result.data, "1");
    }

    if (m != NULL) {
        for (int i = 0; i < strlen(remaining.data); i++) {
            m[i] = remaining.data[i];
        }
        m[strlen(remaining.data)] = '\0';
    }

    free_dynStr(&remaining);

    return result;
}

dynStr exec_mod(scanner *scanner, short base, char *arg1, char *arg2, bool *ok) {
    char mod[40];
    exec_div(scanner, base, arg1, arg2, mod, ok);

    dynStr result;
    init_dynStr(&result);

    for (int i = 0; i < strlen(mod); i++) {
        append_char(&result, mod[i]);
    }

    return result;
}

dynStr exec_pow(scanner *scanner, short base, char *arg1, char *arg2) {
    dynStr result;
    init_dynStr(&result);

    if (arg2[0] == '0') {
        append_char(&result, '1');
        return result;
    }
    // TODO: implement change base first
    // unsigned long long int can store 2^63-1 and we need (16^10=2^40)
}

dynStr exec_convert(scanner *scanner, short fromBase, short toBase, char *arg) {
    dynStr result;
    init_dynStr(&result);
    append_char(&result, '0');

    int len = strlen(arg);
    char fromBase_str[2];
    sprintf(fromBase_str, "%d", fromBase);

    for (int i = 0; i < len; i++) {
        char digit = arg[i];
        int digitValue = char_to_dec(scanner, digit, fromBase, NULL);
        char value_str[2];
        sprintf(value_str, "%d", digitValue);

        dynStr tempResult =
            exec_mul(scanner, toBase, result.data, fromBase_str);
        dynStr tempSum = exec_add(scanner, toBase, tempResult.data, value_str);

        free_dynStr(&result);
        result = tempSum;
        free_dynStr(&tempResult);
    }

    return result;
}

dynStr execute(scanner *scanner, oper op, char *arg1, char *arg2, bool *ok) {
    switch (op.op_type) {
    case Add:
        return exec_add(scanner, op.base, arg1, arg2);
    case Mul:
        return exec_mul(scanner, op.base, arg1, arg2);
    case Pow:
        return exec_pow(scanner, op.base, arg1, arg2);
    case Div:
        return exec_div(scanner, op.base, arg1, arg2, NULL, ok);
    case Mod:
        return exec_mod(scanner, op.base, arg1, arg2, ok);
    case Convert:
        return exec_convert(scanner, op.base, atoi(arg1), arg2);
    }
}
