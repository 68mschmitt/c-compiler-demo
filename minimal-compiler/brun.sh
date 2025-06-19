#!/bin/bash

cd bin
echo "Compiling program.c"
gcc -o program ../program.c
echo "Running program.c"
./program

echo "Linking output assembly"
gcc -o output output.s
echo "Running output assembly"
./output
echo $?
