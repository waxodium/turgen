#include <termios.h>

#ifndef navigation
#define navigation


#include "render.h"

extern struct termios Terminal;

int fclear(char **argv, ShellState *state);
int fexit(char **argv, ShellState *state);
int directory(const char *path);
int cd(char **argv, ShellState *state);

#endif
