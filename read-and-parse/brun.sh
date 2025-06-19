#!/bin/bash

cd bin
gcc -o program ../program.c
./program ../input.c
gcc -o output output.s
echo "Result of executed, compiled, code"
./output
echo $?
