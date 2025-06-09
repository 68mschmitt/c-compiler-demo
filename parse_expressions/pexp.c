#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOKEN_INT,          // 'int'
    TOKEN_RETURN,       // 'return'
    TOKEN_IDENTIFIER,   // main, variable names
    TOKEN_NUMBER,       // 42, 123
    TOKEN_SYMBOL,       // '+', '-', '(', ')', '{', '}', ';', etc.
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[64];     // the actual text of the token
    int   value;    // only used if type == TOKEN_NUMBER
} Token;

const char* keywords[] = { "int", "return", NULL };

void lex(const char* src, Token* tokens, int* token_count) {
    const char* p = src;
    int idx = 0;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (isalpha(*p) || *p == '_') {
            // Start of identifier/keyword
            const char* start = p;
            while (isalnum(*p) || *p == '_') p++;
            int len = p - start;
            strncpy(tokens[idx].text, start, len);
            tokens[idx].text[len] = '\0';

            // Check if keyword
            if (strcmp(tokens[idx].text, "int") == 0) {
                tokens[idx].type = TOKEN_INT;
            } else if (strcmp(tokens[idx].text, "return") == 0) {
                tokens[idx].type = TOKEN_RETURN;
            } else {
                tokens[idx].type = TOKEN_IDENTIFIER;
            }
            idx++;
            continue;
        }

        if (isdigit(*p)) {
            const char* start = p;
            while (isdigit(*p)) p++;
            int len = p - start;
            strncpy(tokens[idx].text, start, len);
            tokens[idx].text[len] = '\0';
            tokens[idx].type = TOKEN_NUMBER;
            tokens[idx].value = atoi(tokens[idx].text);
            idx++;
            continue;
        }

        if (strchr("();{}+-*/", *p)) {
            tokens[idx].type = TOKEN_SYMBOL;
            tokens[idx].text[0] = *p;
            tokens[idx].text[1] = '\0';
            idx++;
            p++;
            continue;
        }

        // Unknown character, skip (or error handling)
        p++;
    }

    tokens[idx].type = TOKEN_EOF;
    *token_count = idx;
}

void print_tokens(Token* tokens, int count) {
    for (int i = 0; i < count; ++i) {
        printf("Token %d: Type %d, Text '%s', Value %d\n",
                i, tokens[i].type, tokens[i].text, tokens[i].value);
    }
}

typedef enum {
    AST_NUMBER,
    AST_BINARY_OP
} ASTNodeType;

typedef enum {
    OP_ADD = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_DIV = 3
} ASTBinaryOpType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        // If type == AST_NUMBER
        int value;

        // If type == AST_BINARY_OP
        struct {
            ASTBinaryOpType op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binop;
    };
} ASTNode;

ASTNode* parse_factor(Token* tokens, int* pos);
ASTNode* parse_term(Token* tokens, int* pos);
ASTNode* parse_expression(Token* tokens, int* pos);

ASTNode* parse_factor(Token* tokens, int* pos) {
    Token* t = &tokens[*pos];

    if (t->type == TOKEN_NUMBER) {
        // Make an AST node for the number
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_NUMBER;
        node->value = t->value;
        (*pos)++;
        return node;
    }

    if (t->type == TOKEN_SYMBOL && strcmp(t->text, "(") == 0) {
        (*pos)++; // consume '('

        ASTNode* node = parse_expression(tokens, pos);

        if (tokens[*pos].type != TOKEN_SYMBOL || strcmp(tokens[*pos].text, ")") != 0) {
            fprintf(stderr, "Expected ')'\n");
            exit(1);
        }
        (*pos)++; // consume ')'
        return node;
    }
    fprintf(stderr, "Unexpected token '%s' in factor\n", t->text);
    exit(1);
}

ASTNode* parse_term(Token* tokens, int* pos) {
    ASTNode* left = parse_factor(tokens, pos);

    while (tokens[*pos].type == TOKEN_SYMBOL &&
            (strcmp(tokens[*pos].text, "*") == 0 || strcmp(tokens[*pos].text, "/") == 0)) {
        ASTBinaryOpType op = (strcmp(tokens[*pos].text, "*") == 0) ? OP_MUL : OP_DIV;
        (*pos)++; // consume operator

        ASTNode* right = parse_factor(tokens, pos);

        // Build a new AST node: left <op> right
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_BINARY_OP;
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node; // treat result as left for chaining (e.g., a*b*c)
    }
    return left;
}

ASTNode* parse_expression(Token* tokens, int* pos) {
    ASTNode* left = parse_term(tokens, pos);

    while (tokens[*pos].type == TOKEN_SYMBOL &&
            (strcmp(tokens[*pos].text, "+") == 0 || strcmp(tokens[*pos].text, "-") == 0)) {
        ASTBinaryOpType op = (strcmp(tokens[*pos].text, "+") == 0) ? OP_ADD : OP_SUB;
        (*pos)++; // consume operator

        ASTNode* right = parse_term(tokens, pos);

        // Build a new AST node: left <op> right
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_BINARY_OP;
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node; // treat result as left for chaining (e.g., a+b+c)
    }
    return left;
}

int eval(ASTNode* node) {
    if (node->type == AST_NUMBER) return node->value;
    int l = eval(node->binop.left);
    int r = eval(node->binop.right);
    switch (node->binop.op) {
        case OP_ADD: return l + r;
        case OP_SUB: return l - r;
        case OP_MUL: return l * r;
        case OP_DIV: return l / r;
    }
    return 0;
}

typedef struct {
    int return_value;
} AST;

int parse(Token* tokens, AST* ast) {
    int pos = 0;

    // 1. int
    if (tokens[pos].type != TOKEN_INT) goto error;
    pos++;

    // 2. main (identifier with text "main")
    if (tokens[pos].type != TOKEN_IDENTIFIER || strcmp(tokens[pos].text, "main")) goto error;
    pos++;

    // 3. (
    if (tokens[pos].type != TOKEN_SYMBOL || strcmp(tokens[pos].text, "(")) goto error;
    pos++;

    // 4. )
    if (tokens[pos].type != TOKEN_SYMBOL || strcmp(tokens[pos].text, ")")) goto error;
    pos++;

    // 5. {
    if (tokens[pos].type != TOKEN_SYMBOL || strcmp(tokens[pos].text, "{")) goto error;
    pos++;

    // 6. return
    if (tokens[pos].type != TOKEN_RETURN) goto error;
    pos++;
    ASTNode* astNode = parse_expression(tokens, &pos);
    ast->return_value = eval(astNode);

    // 8. ;
    if (tokens[pos].type != TOKEN_SYMBOL || strcmp(tokens[pos].text, ";")) goto error;
    pos++;

    // 9. }
    if (tokens[pos].type != TOKEN_SYMBOL || strcmp(tokens[pos].text, "}")) goto error;
    pos++;

    // Success!
    return 1;

error:
    fprintf(stderr, "Parse error at token %d: '%s'\n", pos, tokens[pos].text);
    return 0;
}

void generate_code(const AST* ast, const char* filename) {
    FILE* out = fopen(filename, "w");
    if (!out) {
        perror("fopen");
        exit(1);
    }
    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");
    fprintf(out, "  movl $%d, %%eax\n", ast->return_value);
    fprintf(out, "  ret\n");
    fclose(out);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <sourcefile>\n", argv[0]);
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

    Token tokens[256];
    int count = 0;
    lex(buf, tokens, &count);

    AST ast;
    if (parse(tokens, &ast)) {
        printf("Parsed successfully! Return value: %d\n", ast.return_value);
        generate_code(&ast, "output.s");
        printf("Assembly written to output.s\n");
    } else {
        printf("Parsing failed.\n");
    }
}
