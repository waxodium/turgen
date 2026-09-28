#ifndef JOBS_H
#define JOBS_H

#include "turgen.h"

#define MAX_JOBS 100
#define MAX_PROCESSES 32

typedef enum 
{
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} JobState;

typedef struct 
{
    pid_t pid;
    int status;
    bool running;
    bool stopped;
} JobProcess;

typedef struct 
{
    int id;
    pid_t pgid;
    JobState state;
    char command[1024];
    JobProcess processes[MAX_PROCESSES];
    int process_count;
} Job;

extern Job job_table[MAX_JOBS];

void add_job(pid_t pgid, const char *command, JobState state);
void remove_job(pid_t pgid);

Job *find_job(int id);
Job *find_job_by_pid(pid_t pid);

void update_process(pid_t pid, int status);

#endif
