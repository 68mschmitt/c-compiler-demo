#!/bin/bash

gcc pexp.c -o ./bin/pexp
./bin/pexp input.c
cat output.s
gcc -o ./bin/program output.s
./bin/program

echo Return value: $?
