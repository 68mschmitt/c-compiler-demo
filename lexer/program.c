#include <stdio.h>

typedef enum {
    TOKEN_INT,
    TOKEN_RETURN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_SYMBOL,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char text[64];
    int value;
} Token;

int main() {
    printf("Hello, World!\n");
    return 0;
}
