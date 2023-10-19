#include "execute.h"
#include "helpers.h"
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

dynStr exec_mul(scanner *scanner, short base, char *arg1, char *arg2) {
    int n1 = strlen(arg1);
    int n2 = strlen(arg2);

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

dynStr exec_convert(scanner *scanner, short fromBase, short toBase,
                    char *arg) {
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
        dynStr tempSum =
            exec_add(scanner, toBase, tempResult.data, value_str);

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
    case Convert:
        return exec_convert(scanner, op.base, atoi(arg1), arg2);
    }
}
