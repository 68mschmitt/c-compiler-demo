#include <stdio.h>

int main(int argc, char **argv) {
    FILE *out = fopen("output.s", "w");
    if (!out) {
        perror("fopen");
        return 1;
    }

    fprintf(out, ".global main\n");
    fprintf(out, "main:\n");
    fprintf(out, "  movl $42, %%eax\n");
    fprintf(out, "  ret\n");

    fclose(out);
    return 0;
}
