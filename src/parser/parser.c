#include "turgen.h"
#include "error.h"
#include "render.h"
#include "ast.h"

static Command* parse_range(Token *tokens, int start, int end);


static bool operator_control(TokenType type) {
    return type == TOKEN_SEMICOLON || type == TOKEN_AND || type == TOKEN_OR || type == TOKEN_PIPE;
}

static bool operator_redir(TokenType type) {
    return type == TOKEN_LESS || type == TOKEN_GREATER || type == TOKEN_DGREATER;
}



static bool attatchedfile(char *text) {
    char *p = text;

    while (isdigit((unsigned char)*p)) {
        p++;
    }

    if (strncmp(p, ">>", 2) == 0) {
        p += 2;
    } else if (*p == '>' || *p == '<') {
        p++;
    }

    return (*p != '\0');
}


// -- Syntax Error

static bool verifySyntax_ERR(Token *token, ShellState *state) {
    _error(ERR_SYNTAX, token->text);
    state->last_status = 2;
    return false;
}

static bool verifySyntax(Token *tokens, int count, ShellState *state) {
    if (count == 0) {
        return true;
    }

    if (operator_control(tokens[0].type)) {
        return verifySyntax_ERR(&tokens[0], state);
    }

    for (int i = 0; i < count; i++) {
        TokenType type = tokens[i].type;

        if (operator_control(type)) {
            if (i + 1 >= count || operator_control(tokens[i + 1].type)) {
                return verifySyntax_ERR(&tokens[i], state);
            }
        }

        if (operator_redir(type) && !attatchedfile(tokens[i].text)) {
            if (i + 1 >= count || operator_control(tokens[i + 1].type) || operator_redir(tokens[i + 1].type)) {
                return verifySyntax_ERR(&tokens[i], state);
            }
        }
    }

    return true;
}





static Command* parse_command(Token *tokens, int start, int end) {
    Command *cmd = create_command(TYPE_BASIC);

    if (!cmd) {
        return NULL;
    }

    int max_items = end - start;

    cmd->args = malloc(sizeof(char *) * (max_items + 1));
    cmd->redirs = malloc(sizeof(Redirection) * max_items);

    if (!cmd->args || !cmd->redirs) {
        free_command(cmd);
        return NULL;
    }

    int args_idx = 0;

    for (int i = start; i < end; i++) {
        if (!operator_redir(tokens[i].type)) {
            cmd->args[args_idx++] = strdup(tokens[i].text);
            continue;
        }

        Redirection r;

        r.SOURCE_FD = 1;
        if (tokens[i].type == TOKEN_LESS) {
            r.SOURCE_FD = 0;
        }

        r.TARGET_FD = -1;
        r.file = NULL;
        r.CLOSE_FD = 0;

        if (tokens[i].type == TOKEN_LESS) {
            r.mode = O_RDONLY;
        } else {
            r.mode = O_WRONLY | O_CREAT | O_TRUNC;
        }

        if (tokens[i].type == TOKEN_DGREATER) {
            r.mode = O_WRONLY | O_CREAT | O_APPEND;
        }

        char *ptoken = tokens[i].text;

        if (isdigit((unsigned char)*ptoken)) {
            r.SOURCE_FD = 0;

            while (isdigit( (unsigned char)*ptoken)) {
                r.SOURCE_FD = r.SOURCE_FD * 10 + (*ptoken++ - '0');
            }
        }

        if (strncmp(ptoken, ">>", 2) == 0) {
            ptoken += 2;
        } else if (*ptoken == '>' || *ptoken == '<') {
            ptoken++;
        }

        if (*ptoken == '\0') {
            if (i + 1 < end) {
                r.file = strdup(tokens[++i].text);
            }

            cmd->redirs[cmd->redir_count++] = r;
            continue;
        }

        if (*ptoken == '&') {
            ptoken++;

            if (*ptoken == '-') {
                r.CLOSE_FD = 1;
                cmd->redirs[cmd->redir_count++] = r;
                continue;
            }

            if (isdigit((unsigned char)*ptoken)) {
                r.TARGET_FD = 0;

                while (isdigit((unsigned char)*ptoken)) {
                    r.TARGET_FD = r.TARGET_FD * 10 + (*ptoken++ - '0');
                }

                cmd->redirs[cmd->redir_count++] = r;
                continue;
            }
        }

        r.file = strdup(ptoken);
        cmd->redirs[cmd->redir_count++] = r;
    }

    cmd->args[args_idx] = NULL;
    cmd->arg_count = args_idx;

    return cmd;
}


/*
    parse_range breaks down the list of tokens.
*/
static Command* parse_range(Token *tokens, int start, int end) {
    if (start >= end) {
        return NULL;
    }

    for (int i = start; i < end; i++) {
        if (tokens[i].type != TOKEN_SEMICOLON) {
            continue;
        }

        Command *cmd = create_command(TYPE_SEMICOLON);

        if (!cmd) {
            return NULL;
        }

        cmd->left = parse_range(tokens, start, i);
        cmd->right = parse_range(tokens, i + 1, end);

        return cmd;
    }

    for (int i = start; i < end; i++) {
        
        if (tokens[i].type != TOKEN_AND && tokens[i].type != TOKEN_OR) {
            continue;
        }

        
        CommandType type;

        
        if (tokens[i].type == TOKEN_AND) {
            type = TYPE_AND;
        } else {
            type = TYPE_OR;
        }

        Command *cmd = create_command(type);

        
        if (!cmd) {
            return NULL;
        }

        cmd->left = parse_range(tokens, start, i);
        cmd->right = parse_range(tokens, i + 1, end);

        return cmd;
    }

    

    for (int i = start; i < end; i++) {
        if (tokens[i].type != TOKEN_PIPE) {
            continue;
        }

        Command *cmd = create_command(TYPE_PIPE);

        if (!cmd) {
            return NULL;
        }

        cmd->left = parse_range(tokens, start, i);
        cmd->right = parse_range(tokens, i + 1, end);

        return cmd;
    }

    return parse_command(tokens, start, end);
}





Command* parse(Token *tokens, int count, ShellState *state) {
    if (!verifySyntax(tokens, count, state)) {
        return NULL;
    }

    return parse_range(tokens, 0, count);
}
