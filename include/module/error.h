#ifndef ERROR_H
#define ERROR_H

typedef enum {
    ERR_SYNTAX,
    ERR_NOT_FOUND,
    ERR_DENIED,
    ERR_BAD_FD,
    ERR_CMD_NOT_FOUND,
    ERR_IS_DIR
} ShellError;

void _error(ShellError type, const char *context);

#endif
