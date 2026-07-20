#ifndef JOBS_H
#define JOBS_H

#include "turgen.h"

typedef struct {
    
    int id;
    pid_t pid;
    int state;
    char command[256];

} Job;

#endif
