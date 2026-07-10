#ifndef LEXER_H
#define LEXER_H


typedef struct {
    char *input;
    size_t position;
    size_t length;
} Lexer;


char lexer_current(Lexer *lexer);
char lexer_peek(Lexer *lexer);
void lexer_forward(Lexer *lexer);

#endif
