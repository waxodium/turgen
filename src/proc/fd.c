#include "fd.h"
#include "ast.h"
#include "error.h"

void fd_state_init(FdState *fdstate) {
    fdstate->count = 0;
}

static bool fd_duplicate(int old_fd, int new_fd, FdState *fdstate) {
    if (old_fd == new_fd) {
        return true;
    }

    if (fdstate) {
        fdstate->backups[fdstate->count].target_fd = new_fd;
        fdstate->backups[fdstate->count].original_fd = dup(new_fd);
        fdstate->count++;
    }

    if (dup2(old_fd, new_fd) < 0) {
        if (errno == EBADF) {
            char ctx[16];
            snprintf(ctx, sizeof(ctx), "%d", old_fd);
            _error(ERR_BAD_FD, ctx);
        }
        return false;
    }

    return true;
}

bool fd_redirs(Command *cmd, FdState *st) {
    for (int i = 0; i < cmd->redir_count; i++) {
        Redirection r = cmd->redirs[i];

        if (r.CLOSE_FD) {
            if (close(r.SOURCE_FD) < 0 && errno == EBADF) {
                char bucket[16];
                snprintf(bucket, sizeof(bucket), "%d", r.SOURCE_FD);
                _error(ERR_BAD_FD, bucket);
                return false;
            }
            continue;
        }

        if (r.file) {
            int fd = open(r.file, r.mode | O_CLOEXEC, 0644);

            if (fd < 0) {
                if (errno == EACCES) {
                    _error(ERR_DENIED, r.file);
                } else if (errno == EISDIR) {
                    _error(ERR_IS_DIR, r.file);
                } else {
                    _error(ERR_NOT_FOUND, r.file);
                }
                return false;
            }

            if (!fd_duplicate(fd, r.SOURCE_FD, st)) {
                if (fd != r.SOURCE_FD) {
                    close(fd);
                }
                return false;
            }

            if (fd != r.SOURCE_FD) {
                close(fd);
            }

            continue;
        }

        if (r.TARGET_FD >= 0) {
            if (!fd_duplicate(r.TARGET_FD, r.SOURCE_FD, st)) {
                return false;
            }
        }
    }

    return true;
}

void fd_restore(FdState *fdstate) {
    for (int i = fdstate->count - 1; i >= 0; i--) {
        int origin = fdstate->backups[i].original_fd;
        int target = fdstate->backups[i].target_fd;

        if (origin >= 0) {
            dup2(origin, target);
            close(origin);
        } else {
            close(target);
        }
    }

    fdstate->count = 0;
}
