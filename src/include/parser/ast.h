#ifndef AST_H
#define AST_H

#include "operator.h"

typedef enum {
    TYPE_BASIC,
    TYPE_PIPE,
    TYPE_AND,
    TYPE_OR,
    TYPE_SEMICOLON
} CommandType;

typedef struct Command {
    CommandType type;

    char **args;
    int arg_count;

    Redirection *redirs;
    int redir_count;

    struct Command *left;
    struct Command *right;
} Command;

Command* create_command(CommandType type);
void free_command(Command *cmd);

#endif
