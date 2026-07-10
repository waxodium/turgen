#include "turgen.h"
#include "sout.h"

#include "error.h"

void _error(ShellError type, const char *context) {
    const char *token = context;
    const char *fallback = "newline";

    if (!token) {
        token = fallback;
    }

    switch (type) {
        case ERR_SYNTAX:
            sout("\r%s: syntax error near unexpected token `%s`\r\n", shellname, token);
            break;
        case ERR_NOT_FOUND:
            sout("\r%s: %s: No such file or directory\r\n", shellname, token);
            break;
        case ERR_DENIED:
            sout("\r%s: %s: Permission denied\r\n", shellname, token);
            break;
        case ERR_BAD_FD:
            sout("\r%s: %s: Bad file descriptor\r\n", shellname, token);
            break;
        case ERR_CMD_NOT_FOUND:
            sout("\r%s: %s: command not found\r\n", shellname, token);
            break;
        case ERR_IS_DIR:
            sout("\r%s: %s: Is a directory\r\n", shellname, token);
            break;
    }
}
