#include <stdio.h>
#include <ctype.h>

// Token types.
#define T_UNKNOWN 0
#define T_LEFT_PAREN 1
#define T_RIGHT_PAREN 2
#define T_SYMBOL 3
#define T_NUMBER 4

// Node types.
#define N_UNKNOWN 0
#define N_NUMBER 1
#define N_SYMBOL 2
#define N_EXPR 3

// Maximum token length.
#define TOKEN_MAX_LEN 16

/**
 * Represents a token.
 */
struct Token {
    int type;
    char* seq;
};

/**
 * Represents a node in a parse tree.
 */
struct Node {
    int type;
    int start;
    struct Node* x;
    struct Node* y;
    struct Node* z;
};

/**
 * Returns the size of a file.
 *
 * @param file  the file to get the size of
 */
long fsize(FILE* file) {
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return size;
}

/**
 * Returns the contents of a file.
 *
 * @param path  the path of the file
 */
char* read(char *path) {
    FILE* file;
    file = fopen(path, "rb");
    long size = fsize(file);
    char* buf = (char*) calloc(1, size + 1);
    fread(buf, size, 1, file);
    return buf;
}

/**
 * Returns the token type of a character.
 *
 * @param chr   the character to get the token type of
 */
int ttype(char chr) {
    int type = T_UNKNOWN;
    if (chr == '(') {
        type = T_LEFT_PAREN;
    }
    else if (chr == ')') {
        type = T_RIGHT_PAREN;
    }
    else if (isdigit(chr)) {
        type = T_NUMBER;
    }
    else if (isalpha(chr)) {
        type = T_SYMBOL;
    }
    return type;
}

/**
 * Returns true if a token type is a single character, otherwise returns false.
 *
 * @param type  the token type to check
 */
int tsingle(int type) {
    return type == T_LEFT_PAREN || type == T_RIGHT_PAREN;
}

/**
 * Emits a token with a name and sequence onto a token list.
 *
 * @param tokens    the token list to emit to
 * @param type      the token type
 * @param seq       the token sequence
 */
void temit(struct Token* tokens, int type, char* seq) {
    int i = 0;
    while (tokens[i] != NULL) {
        i++; // Dangerously seek end of array.
    }

    // Add new token at next available index.
    struct Token token;
    token.type = type;
    token.seq = seq;
    tokens[i] = token;
}

/**
 * Appends a character on the end of a string.
 *
 * @param string    the string to append to
 * @param chr       the character to append
 */
void append(char* string, char chr) {
    int i = 0;
    while (string[i] != NULL) {
        i++; // Dangerously seek end of string.
    }

    // Add new character at next available index.
    string[i] = chr;
}

/**
 * Counts the number of tokens in a source string.
 *
 * @param source    the source string
 */
int tmeasure(char* source) {
    int count = 0; // We need to count the number of tokens in the string.

    // Count tokens in source.
    int len = strlen(source);
    int prev = T_UNKNOWN;
    for (int i = 0; i < len; i++) {
        int curr = ttype(source[i]);
        if (prev != T_UNKNOWN && (tsingle(prev) || curr != prev)) {
            count++;
        }
        prev = curr;
    }

    // Count last token if not T_UNKNOWN.
    return count + (prev == T_UNKNOWN ? 0 : 1);
}

/**
 * Tokenizes a string of source code.
 *
 * @param source    the string of source code to tokenize
 */
struct Token* tokenize(char* source) {
    int size = tmeasure(source); // Get number of tokens in source.

    // Allocate space for that number of tokens.
    struct Token* tokens = (struct Token*) calloc(sizeof(struct Token), size);

    char* seq = (char*) calloc(1, TOKEN_MAX_LEN); // Hold current token text.

    int len = strlen(source);
    int prev = T_UNKNOWN;
    for (int i = 0; i < len; i++) {
        int curr = ttype(source[i]); // Current token type.

        // If token boundary is crossed.
        if (prev != T_UNKNOWN && (tsingle(prev) || curr != prev)) {
            temit(tokens, prev, seq); // Emit token.
            seq = (char*) calloc(1, TOKEN_MAX_LEN); // Fresh token text.
        }

        // Append to sequence if not T_UNKNOWN.
        if (curr != T_UNKNOWN) {
            append(seq, source[i]);
        }

        prev = curr;
    }

    // Emit last token if not T_UNKNOWN.
    if (prev != T_UNKNOWN) {
        temit(tokens, prev, seq);
    }

    return tokens;
}

/**
 * Returns the starting index of the next expression in a token list.
 *
 * @param tokens  the token list
 * @param start   the index to start at
 */
int nextexp(struct Token* tokens, int start) {
    int i = start;
    if (tokens[start].type == T_LEFT_PAREN) {
        int b = 1; // Brackets better be balanced.
        while (b != 0) {
            i++;
            if (tokens[i].type == T_LEFT_PAREN) {
                b++;
            }
            else if (tokens[i].type == T_RIGHT_PAREN) {
                b--;
            }
        }
    }
    return i + 1;
}

/**
 * Parses a list of tokens into an expession tree.
 *
 * @param tokens    the token list to parse
 */
struct Node* parse(struct Node* node, struct Token* tokens) {
    switch (tokens[node->start].type) { // Assign type to node.
        case T_LEFT_PAREN:
            node->type = N_EXPR;
            break;
        case T_NUMBER:
            node->type = N_NUMBER;
            break;
        case T_SYMBOL:
            node->type = N_SYMBOL;
            break;
    }

    // If it's a list expression we should read it.
    if (node->type == N_EXPR) {

        // Add operator node.
        struct Node* x = (struct Node*) calloc(sizeof(struct Node), 1);
        x->type = N_UNKNOWN;
        x->start = node->start + 1;
        node->x = x;
        parse(x, tokens);

        // Add left operand node.
        struct Node* y = (struct Node*) calloc(sizeof(struct Node), 1);
        int sy = nextexp(tokens, node->start + 1);
        y->type = N_UNKNOWN;
        y->start = sy;
        node->y = y;
        parse(y, tokens);

        // Add right operand node.
        struct Node* z = (struct Node*) calloc(sizeof(struct Node), 1);
        int sz = nextexp(tokens, sy);
        z->type = N_UNKNOWN;
        z->start = sz;
        node->z = z;
        parse(z, tokens);
    }
}

/**
 * Compiles a node to an output file.
 *
 * @param node    the node to compile
 * @param tokens  the tokenized source
 * @param file    the file to compile to
 */
void compile(struct Node* node, struct Token* tokens, FILE* file) {
    switch (node->type) {
        case N_NUMBER: // Numbers are pushed to the stack.
            fprintf(file, "ldc %s\n", tokens[node->start].seq);
            break;
        case N_SYMBOL: // Symbols are assumed to be operators.
            fprintf(file, "%s\n", tokens[node->start].seq);
            break;
        case N_EXPR: // Asssume all operators are binary.
            compile(node->y, tokens, file); // Push left operand.
            compile(node->z, tokens, file); // Push right operand.
            compile(node->x, tokens, file); // Execute operator.
            break;
    }
}

main() {
    // Read in source.
    char* source = read("example.math");
    printf("Source: %s\n", source);

    // Count tokens.
    int size = tmeasure(source);
    printf("Tokens: %d\n", size);

    // Tokenize source.
    struct Token* tokens = tokenize(source);
    for (int i = 0; i < size; i++) {
        printf("Type: %d | Sequence: %s\n", tokens[i].type, tokens[i].seq);
    }

    // Define root node.
    struct Node* root = (struct Node*) calloc(sizeof(struct Node), 1);
    root->type = N_UNKNOWN; // Must be cons.
    root->start = 0;

    parse(root, tokens); // Recursively parse.

    // Compile to file.
    FILE* file = fopen("example.ssm", "w");
    compile(root, tokens, file);
    fclose(file);

    // TODO: Deallocate memory.
}
