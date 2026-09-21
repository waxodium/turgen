#ifndef JOBS_H
#define JOBS_H

#include "turgen.h"

typedef enum {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE,
    JOB_TERMINATED
} JobState;

typedef struct {
    int id;
    pid_t pid;
    pid_t pgid;
    JobState state;
    char command[256];
    int status;
} Job;

void jobs_init(void);
void jobs_clear(void);

Job *job_create(pid_t pid, const char *command);
Job *job_get(int id);
Job *job_get_pid(pid_t pid);

int job_update(pid_t pid, int status);

int job_remove(int id);
int job_kill(int id, int signal);

int jobs_reap(void);
int job_wait(int id);

size_t jobs_count(void);
Job *jobs_at(size_t index);

const char *job_state_string(JobState state);

#endif
