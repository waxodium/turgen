#ifndef QUOTE_H
#define QUOTE_H

#include "turgen.h"


typedef struct {
    int SOURCE_FD;
    int TARGET_FD;
    char *file;
    int mode;
    int CLOSE_FD;
} Redirection;

typedef enum {
    TOKEN_WORD,
    TOKEN_SEMICOLON,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_PIPE,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_DGREATER
} TokenType;

typedef struct {
    char *text;
    TokenType type;
} Token;


Token *tokenize(char *input);

#endif
