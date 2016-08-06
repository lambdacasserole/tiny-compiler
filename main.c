#include <stdio.h>
#include <ctype.h>

// Token types.
#define UNKNOWN 0
#define LEFT_PAREN 1
#define RIGHT_PAREN 2
#define SYMBOL 3
#define NUMBER 4

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

    int len = strlen(source);
    int prev = UNKNOWN;
    char* seq = (char*) calloc(1, TOKEN_MAX_LEN);
    for (int i = 0; i < len; i++) {
        int cur = ttype(source[i]);
        if (prev != UNKNOWN) {
            if (tsingle(prev) || cur != prev) {
                temit(tokens, prev, seq); // Emit token.
                seq = (char*) calloc(1, TOKEN_MAX_LEN);
            }
        }
        if (cur != UNKNOWN) {
            append(seq, source[i]); // Append to sequence.
        }
        prev = cur;
    }

    // Emit last token if not unknown.
    if (prev != UNKNOWN) {
        temit(tokens, prev, seq);
    }

    return tokens;
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

    // TODO: Deallocate memory.
}
