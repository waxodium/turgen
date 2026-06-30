#ifndef globs
#define globs

#include "turgen.h"

typedef struct {
    char **paths;
    int count;
} GlobResult;

void freeGlob(GlobResult *result);

GlobResult globbing(const char *pattern);

#endif
