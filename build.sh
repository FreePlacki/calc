#!/bin/sh

set -xe

gcc -pedantic -Wall -Wextra -o main helpers.c main.c
