#!/bin/sh

set -x

compiler=clang

$compiler main.c -o raycast_demo -Wall -Wextra $(pkg-config --libs --cflags raylib) -lm
