#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <sourcefile\n>", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("fopen input");
        return 1;
    }

    char buf[1024];
    size_t len = fread(buf, 1, sizeof(buf)-1, in);
    buf[len] = '\0';
    fclose(in);

    char *ret = strstr(buf, "return");
    if (!ret) {
        fprintf(stderr, "No return statement found.\n");
        return 1;
    }
    ret += 6;

    while (*ret && isspace((unsigned char) *ret)) ret++;

    int value = atoi(ret);

    FILE *out = fopen("output.s", "w");
    if (!out) {
        perror("fopen output");
        return 1;
    }

    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");
    fprintf(out, "  movl $%d, %%eax\n", value);
    fprintf(out, "  ret\n");
    
    fclose(out);
    return 0;
}
