#!/bin/bash

gcc -o bin/program program.c
cd ./bin
./program ../input.c
