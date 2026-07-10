#include "turgen.h"
#include "ast.h"

Command* create_command(CommandType type) {
    Command *cmd = malloc(sizeof(Command));

    if (!cmd) {
        return NULL;
    }

    cmd->type = type;
    cmd->args = NULL;
    cmd->arg_count = 0;
    cmd->redirs = NULL;
    cmd->redir_count = 0;
    cmd->left = NULL;
    cmd->right = NULL;

    return cmd;
}




/*
    free() to builtins and external.
    
    origins from all of
    
    - char **args (struct Command)
    
    `struct Command;`:
        - *left
        - *right

    including (struct Command & struct Redirection)
*/

void free_command(Command *cmd) {
    if (!cmd) {
        return;
    }

    if (cmd->left) {
        free_command(cmd->left);
    }

    if (cmd->right) {
        free_command(cmd->right);
    }

    if (cmd->args) {
        for (int i = 0; i < cmd->arg_count; i++) {
            if (cmd->args[i]) {
                free(cmd->args[i]);
            }
        }

        free(cmd->args);
    }

    if (cmd->redirs) {
        for (int i = 0; i < cmd->redir_count; i++) {
            if (cmd->redirs[i].file) {
                free(cmd->redirs[i].file);
            }
        }

        free(cmd->redirs);
    }

    free(cmd);
}
