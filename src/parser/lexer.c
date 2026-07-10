#include "turgen.h"
#include "lexer.h"

char lexer_current(Lexer *lexer) {
    if (lexer->position >= lexer->length) return '\0';
    return lexer->input[lexer->position];
}

char lexer_peek(Lexer *lexer) {
    if (lexer->position + 1 >= lexer->length) return '\0';
    return lexer->input[lexer->position + 1];
}

void lexer_forward(Lexer *lexer) {
    if (lexer->position < lexer->length) {
        lexer->position++;
    }
}
