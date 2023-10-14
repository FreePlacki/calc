#!/bin/sh

set -xe

gcc -pedantic -Wall -Wextra -g -o calc scanner.c error.c helpers.c main.c
