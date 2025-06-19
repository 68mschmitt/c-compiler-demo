#include <stdio.h>

int main(int argc, char **argv) {
    // For now, ignore input and always output the same code
    FILE *out = fopen("output.s", "w");
    if (!out) {
        perror("fopen");
        return 1;
    }

    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");
    fprintf(out, "  movl $42, %%eax\n");
    fprintf(out, "  ret\n");

    fclose(out);
    return 0;
}
