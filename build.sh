#!/bin/sh

set -xe

#gcc -pedantic -Wall -Wextra -g -o calc execute.c scanner.c error.c helpers.c main.c
gcc -O2 -pedantic -Wall -Wextra -o calc execute.c scanner.c error.c helpers.c main.c
