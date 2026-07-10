#ifndef FD_H
#define FD_H
#include "ast.h"

typedef struct {
    int original_fd;
    int target_fd;
} FdBackup;

typedef struct {
    FdBackup backups[32];
    int count;
} FdState;

void fd_state_init(FdState *fdstate);
bool fd_redirs(Command *cmd, FdState *fdstate);
void fd_restore(FdState *fdstate);

#endif
