#!/bin/bash

CC=clang
CFLAGS="-O3 -Wall -mavx2 -msse4.2"
SRC_DIR="src"
OUTPUT="text_parser"
DATA_FILE="input.txt"

$CC $CFLAGS "$SRC_DIR"/*.cpp -o $OUTPUT

if [ $? -eq 0 ]; then
    if [ -f "$DATA_FILE" ]; then
        ./$OUTPUT "$DATA_FILE"
    else
        ./$OUTPUT
    fi
else
    echo "Compilation failed."
    exit 1
fi