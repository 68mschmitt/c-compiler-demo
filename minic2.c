#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv) {
    if (argc !=2) {
        fprintf(stderr, "Usage: %s <sourcefile>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("fopen input");
        return 1;
    }

    // Read file into buffer (assuming it's small)
    char buf[1024];
    size_t len = fread(buf, 1, sizeof(buf)-1, in);
    buf[len] = '\0';
    fclose(in);

    // Very naive search for "return <int>;"
    char *ret = strstr(buf, "return");
    if (!ret) {
        fprintf(stderr, "No return statement found.\n");
        return 1;
    }
    ret += 6; // move past 'return'

    while (*ret && isspace((unsigned char)*ret)) ret++;

    int value = atoi(ret); // get integer after return

    // Write the assembly output
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
