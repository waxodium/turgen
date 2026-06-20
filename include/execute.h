#ifndef EXECUTE
#define EXECUTE

#include "navigation.h"

typedef int (*Handler)(char **, ShellState *);

typedef struct {
    const char *name;
    Handler func;
} Command;

static Command builtins[] = {
    {"clear", tclear},
    {"cls",   tclear},
    {"exit",  texit}, 
    {"cd",    cd}
};

#endif
