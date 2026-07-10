#include "turgen.h"
#include "error.h"
#include "sout.h"

#include "lexer.h"
#include "operator.h"

#define LIMIT 32

static bool is_operator(char c) {
    return c == '&' || c == '|' || c == ';' || c == '<' || c == '>';
}

static Token create_token(char *text, TokenType type) {
    Token t;
    t.text = strdup(text);
    t.type = type;
    return t;
}

static int matchoperator(Token *t, char c, char next) {
    if (c == '&' && next == '&') { t->text = strdup("&&"); t->type = TOKEN_AND; return 2; }
    if (c == '|' && next == '|') { t->text = strdup("||"); t->type = TOKEN_OR; return 2; }
    if (c == '>' && next == '>') { t->text = strdup(">>"); t->type = TOKEN_DGREATER; return 2; }
    
    if (c == ';') { t->text = strdup(";"); t->type = TOKEN_SEMICOLON; return 1; }
    if (c == '|') { t->text = strdup("|"); t->type = TOKEN_PIPE; return 1; }
    if (c == '<') { t->text = strdup("<"); t->type = TOKEN_LESS; return 1; }
    if (c == '>') { t->text = strdup(">"); t->type = TOKEN_GREATER; return 1; }
    return 0;
}


Token *tokenize(char *input) {
    if (!input) {
        return NULL;
    }

    Lexer lexer = {input, 0, strlen(input)};

    int token_capacity = 32;
    int token_count = 0;

    Token *tokens = malloc(token_capacity * sizeof(Token));
    char *word = malloc(lexer.length * 2 + 1);

    int word_length = 0;
    char quote = '\0';
    bool word_building = false;

    if (!tokens || !word) {
        free(tokens);
        free(word);
        return NULL;
    }

    while (lexer.position < lexer.length) {
        if (token_count >= token_capacity - 1) {
            token_capacity *= 2;

            Token *tmp = realloc(tokens, token_capacity * sizeof(Token));

            if (!tmp) {
                free(word);
                free(tokens);
                return NULL;
            }

            tokens = tmp;
        }

        char c = lexer_current(&lexer);

        if (quote == '\'') {
            word_building = true;

            if (c == '\'') {
                quote = '\0';
            } else {
                word[word_length++] = c;
            }

            lexer_forward(&lexer);
            continue;
        }
        


        if (quote == '"') {
            word_building = true;

            if (c == '"') {
                quote = '\0';
                lexer_forward(&lexer);
                continue;
            }

            if (c == '\\') {
                char next = lexer_peek(&lexer);

                if (next != '\0' && strchr("\"\\$\n", next)) {
                    if (next != '\n') {
                        word[word_length++] = next;
                    }
                    lexer_forward(&lexer); 
                    lexer_forward(&lexer); 
                    continue;
                }
            }

            word[word_length++] = c;
            lexer_forward(&lexer);
            continue;
        }

        if (isspace((unsigned char)c)) {
            if (word_building) {
                word[word_length] = '\0';
                tokens[token_count++] = create_token(word, TOKEN_WORD);
                word_length = 0;
                word_building = false;
            }

            lexer_forward(&lexer);
            continue;
        }

        size_t lookahead = lexer.position;

        while (lookahead < lexer.length && isdigit((unsigned char)lexer.input[lookahead])) {
            lookahead++;
        }

        char redirection_check = '\0';

        if (lookahead < lexer.length) {
            redirection_check = lexer.input[lookahead];
        }

        if (redirection_check == '<' || redirection_check == '>') {
            if (word_building) {
                word[word_length] = '\0';
                tokens[token_count++] = create_token(word, TOKEN_WORD);
                word_length = 0;
                word_building = false;
            }

            char redirection_buffer[128];
            int redirection_length = 0;

            while (lexer.position < lookahead && redirection_length < 127) {
                redirection_buffer[redirection_length++] = lexer_current(&lexer);
                lexer_forward(&lexer);
            }

            TokenType type = TOKEN_GREATER;
            char symbol = lexer_current(&lexer);

            if (symbol == '<') {
                type = TOKEN_LESS;
                redirection_buffer[redirection_length++] = symbol;
                lexer_forward(&lexer);
            } else if (symbol == '>') {
                redirection_buffer[redirection_length++] = symbol;
                lexer_forward(&lexer);

                if (lexer_current(&lexer) == '>') {
                    type = TOKEN_DGREATER;
                    redirection_buffer[redirection_length++] = '>';
                    lexer_forward(&lexer);
                }
            }

            if (lexer_current(&lexer) == '&') {
                redirection_buffer[redirection_length++] = '&';
                lexer_forward(&lexer);

                if (lexer_current(&lexer) == '-') {
                    redirection_buffer[redirection_length++] = '-';
                    lexer_forward(&lexer);
                } else {
                    while (isdigit((unsigned char)lexer_current(&lexer)) &&
                           redirection_length < 127) {
                        redirection_buffer[redirection_length++] = lexer_current(&lexer);
                        lexer_forward(&lexer);
                    }
                }
            }

            redirection_buffer[redirection_length] = '\0';
            tokens[token_count++] = create_token(redirection_buffer, type);

            continue;
        }

        if (is_operator(c)) {
            if (word_building) {
                word[word_length] = '\0';
                tokens[token_count++] = create_token(word, TOKEN_WORD);
                word_length = 0;
                word_building = false;
            }

            char next = lexer_peek(&lexer);
            int consumed = matchoperator(&tokens[token_count], c, next);

            if (consumed > 0) {
                token_count++;

                lexer_forward(&lexer);

                if (consumed == 2) {
                    lexer_forward(&lexer);
                }
            }

            continue;
        }

        word_building = true;

        if (c == '\\') {
            char next = lexer_peek(&lexer);

            if (next == '\n') {
                lexer_forward(&lexer);
            } else if (next != '\0') {
                word[word_length++] = next;
                lexer_forward(&lexer);
            } else {
                word[word_length++] = '\\';
            }

            lexer_forward(&lexer);
            continue;
        }

        if (c == '\'') {
            quote = '\'';
            lexer_forward(&lexer);
            continue;
        }

        if (c == '"') {
            quote = '"';
            lexer_forward(&lexer);
            continue;
        }

        word[word_length++] = c;
        lexer_forward(&lexer);
    }

    if (quote != '\0') {
        free(word);
        free(tokens);
        return NULL;
    }

    if (word_building) {
        word[word_length] = '\0';
        tokens[token_count++] = create_token(word, TOKEN_WORD);
    }

    tokens[token_count].text = NULL;

    free(word);

    return tokens;
}
