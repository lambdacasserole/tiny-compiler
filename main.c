#include <stdio.h>
#include <ctype.h>

// Token types.
#define UNKNOWN 0
#define LEFT_PAREN 1
#define RIGHT_PAREN 2
#define SYMBOL 3
#define NUMBER 4

// Node types.
#define N_UNKNOWN 0
#define N_NUMBER 2
#define N_SYMBOL 3
#define N_EXPR 4

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
    struct Node* operator;
    struct Node* x;
    struct Node* y;
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
    int type = UNKNOWN;
    if (chr == '(') {
        type = LEFT_PAREN;
    }
    else if (chr == ')') {
        type = RIGHT_PAREN;
    }
    else if (isdigit(chr)) {
        type = NUMBER;
    }
    else if (isalpha(chr)) {
        type = SYMBOL;
    }
    return type;
}

/**
 * Returns true if a token type is a single character, otherwise returns false.
 *
 * @param type  the token type to check
 */
int tsingle(int type) {
    return type == LEFT_PAREN || type == RIGHT_PAREN;
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
    int prev = UNKNOWN;
    for (int i = 0; i < len; i++) {
        int curr = ttype(source[i]);
        if (prev != UNKNOWN && (tsingle(prev) || curr != prev)) {
            count++;
        }
        prev = curr;
    }

    // Count last token if not unknown.
    return count + (prev == UNKNOWN ? 0 : 1);
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
    int prev = UNKNOWN;
    for (int i = 0; i < len; i++) {
        int curr = ttype(source[i]); // Current token type.

        // If token boundary is crossed.
        if (prev != UNKNOWN && (tsingle(prev) || curr != prev)) {
            temit(tokens, prev, seq); // Emit token.
            seq = (char*) calloc(1, TOKEN_MAX_LEN); // Fresh token text.
        }

        // Append to sequence if not unknown.
        if (curr != UNKNOWN) {
            append(seq, source[i]);
        }

        prev = curr;
    }

    // Emit last token if not unknown.
    if (prev != UNKNOWN) {
        temit(tokens, prev, seq);
    }

    return tokens;
}

int nextexp(struct Token* tokens, int start) {
    if (tokens[start].type == LEFT_PAREN) {
        int i = 1; // Brackets better be balanced.
        int p = start;
        while (i != 0) {
            p++;
            if (tokens[p].type == LEFT_PAREN) {
                i++;
            }
            else if (tokens[p].type == RIGHT_PAREN) {
                i--;
            }
        }
        return p + 1;
    }
    return start + 1;
}

/**
 * Parses a list of tokens into an expession tree.
 *
 * @param tokens    the token list to parse
 */
struct Node* parse(struct Node* node, struct Token* tokens) {
    switch (tokens[node->start].type) { // Assign type to node.
        case LEFT_PAREN:
            node->type = N_EXPR;
            break;
        case NUMBER:
            node->type = N_NUMBER;
            break;
        case SYMBOL:
            node->type = N_SYMBOL;
            break;
    }

    // If it's a list expression we should read it.
    if (node->type == N_EXPR) {

        // Add operator node.
        struct Node* op = (struct Node*) calloc(sizeof(struct Node), 1);
        op->type = N_UNKNOWN;
        op->start = node->start + 1;
        node->operator = op;
        parse(op, tokens);

        // Add left operand node.
        struct Node* x = (struct Node*) calloc(sizeof(struct Node), 1);
        int sx = nextexp(tokens, node->start + 1);
        x->type = N_UNKNOWN;
        x->start = sx;
        node->x = x;
        parse(x, tokens);

        // Add right operand node.
        struct Node* y = (struct Node*) calloc(sizeof(struct Node), 1);
        int sy = nextexp(tokens, sx);
        y->type = N_UNKNOWN;
        y->start = sy;
        node->y = y;
        parse(y, tokens);
    }
}

void compile(struct Node* node, struct Token* tokens) {
    switch (node->type) {
        case N_NUMBER:
            printf("push %s\n", tokens[node->start].seq);
            break;
        case N_SYMBOL:
            printf("%s\n", tokens[node->start].seq);
            break;
        case N_EXPR:
            compile(node->x, tokens);
            compile(node->y, tokens);
            compile(node->operator, tokens);
            break;
    }
}

main() {
    // Read in source.
    char* source = read("test.txt");
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
    compile(root, tokens);

    // TODO: Deallocate memory.
}
