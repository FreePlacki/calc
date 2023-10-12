#!/bin/sh

set -xe

gcc -pedantic -Wall -Wextra -g -o main error.c helpers.c main.c
