#include "turgen.h"
#include "error.h"
#include "execute.h"
#include "terminal.h"
#include "sout.h"

#include "globs.h"
#include "paths.h"
#include "fd.h"
#include "parser.h"

#include "operator.h"

static int external(char **final);
static bool internal(char *command, char **argv, ShellState *state);
static int expanding(char **argv, int argc, char **final);
static int execute_ast(Command *cmd, ShellState *state);

int total(void) {
    return sizeof(builtins) / sizeof(innate);
}

static bool run_builtin(char *name, char **argv, ShellState *state) {
    for (int i = 0; i < total(); i++) {
        if (strcmp(name, builtins[i].name) == 0) {
            state->last_status = builtins[i].func(argv, state);
            return true;
        }
    }
    return false;
}

static bool internal(char *command, char **argv, ShellState *state) {
    struct stat info;
    char path[1024];
    bool is_path = false;

    if (strcmp(command, "..") == 0 || strcmp(command, "...") == 0) {
        is_path = true;
    }

    if (strchr(command, '/') != NULL) is_path = true;
    if (command[0] == '~') is_path = true;



    if (is_path) {
        if (strcmp(command, "...") == 0) {
            snprintf(path, sizeof(path), "../..");
        } 

        else if(command[0] == '~') {
            
            char *home = getenv("HOME");
            if (!home) {
                home = "";
            }
            
            snprintf(path, sizeof(path), "%s%s", home, command + 1);
        
        } else if (command[0] != '/' && command[0] != '.') {
            joinPath(path, sizeof(path), ".", command);
        
        } else {
            strncpy(path, command, sizeof(path) - 1);
            path[sizeof(path) - 1] = '\0';
        }

        if (stat(path, &info) == 0 && S_ISDIR(info.st_mode)) {
            char *args[] = {"cd", path, NULL};
            return run_builtin("cd", args, state);
        }
        

    }


    return run_builtin(command, argv, state);
}

static int expanding(char **argv, int argc, char **final) {
    int count = 0;
    for (int i = 0; i < argc; i++) {
        if (strchr(argv[i], '*') || strchr(argv[i], '?')) {
            GlobResult match = globbing(argv[i]);
            for (int j = 0; j < match.count && count < 1023; j++) {
                final[count++] = match.paths[j];
            }
        } else {
            final[count++] = argv[i];
        }
    }
    final[count] = NULL;
    return count;
}

static int external(char **final) {
    pid_t pid = fork();

    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

        execvp(final[0], final);

        if (errno == ENOENT) {
            _error(ERR_CMD_NOT_FOUND, final[0]);
            exit(127);
        } else if (errno == EACCES) {
            struct stat st;
            if (stat(final[0], &st) == 0 && S_ISDIR(st.st_mode)) {
                _error(ERR_IS_DIR, final[0]);
            } else {
                _error(ERR_DENIED, final[0]);
            }
            exit(126);
        }

        exit(1);
    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }

    return 1;
}

static int runner(Command *cmd, ShellState *state) {
    FdState fd_state;
    fd_state_init(&fd_state);

    if (!fd_redirs(cmd, &fd_state)) {
        fd_restore(&fd_state);
        state->last_status = 1;
        return 1;
    }

    int status = 0;

    if (internal(cmd->args[0], cmd->args, state)) {
        fflush(stdout);
        fflush(stderr);

        fd_restore(&fd_state);
        status = state->last_status;
    } else {
        char *final[1024];
        expanding(cmd->args, cmd->arg_count, final);
        status = external(final);
    }

    fd_restore(&fd_state);
    return status;
}

static int execute_ast(Command *cmd, ShellState *state) {
    if (!cmd) return 0;

    if (cmd->type == TYPE_SEMICOLON) {
        execute_ast(cmd->left, state);
        return execute_ast(cmd->right, state);
    }

    if (cmd->type == TYPE_AND) {
        int res = execute_ast(cmd->left, state);
        if (res == 0) {
            return execute_ast(cmd->right, state);
        }
        return res;
    }

    if (cmd->type == TYPE_OR) {
        int res = execute_ast(cmd->left, state);
        if (res != 0) {
            return execute_ast(cmd->right, state);
        }
        return res;
    }

    if (cmd->type == TYPE_PIPE) {
        int links[2];
        if (pipe(links) < 0) return 1;

        pid_t child1 = fork();
        if (child1 == 0) {
            dup2(links[1], 1);
            close(links[0]);
            close(links[1]);
            exit(execute_ast(cmd->left, state));
        }

        pid_t child2 = fork();
        if (child2 == 0) {
            dup2(links[0], 0);
            close(links[0]);
            close(links[1]);
            exit(execute_ast(cmd->right, state));
        }

        close(links[0]);
        close(links[1]);

        int status1, status2;
        waitpid(child1, &status1, 0);
        waitpid(child2, &status2, 0);

        if (WIFEXITED(status2)) {
            state->last_status = WEXITSTATUS(status2);
            return state->last_status;
        }

        return 1;
    }

    if (cmd->type == TYPE_BASIC) {
        return runner(cmd, state);
    }

    return 0;
}

void execute(char *buffer, ShellState *state) {
    if (!buffer) return;
    if (*buffer == '\0') return;

    Token *tokens = tokenize(buffer);
    if (!tokens) return;

    int count = 0;
    while (tokens[count].text) {
        count++;
    }

    if (count == 0) {
        free(tokens);
        return;
    }

    Command *ast = parse(tokens, count, state);
    if (ast) {
        disableRaw(&Terminal);
        execute_ast(ast, state);
        enableRaw(&Terminal);
        free_command(ast);
    }

    for (int i = 0; i < count; i++) {
        free(tokens[i].text);
    }

    free(tokens);
}

