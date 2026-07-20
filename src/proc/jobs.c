#include "turgen.h"
#include "jobs.h"

#define MAX_JOBS 100


Job job_table[MAX_JOBS] = {0};
static int next_id = 1;

void add_job(pid_t pid, const char *command, int state) {
    if (pid <= 0) return;

    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].pid == 0) {
            job_table[i].pid = pid;
            job_table[i].id = next_id++;
            job_table[i].state = state;
            
            strncpy(job_table[i].command, command, sizeof(job_table[i].command) - 1);
            job_table[i].command[sizeof(job_table[i].command) - 1] = '\0';
            return;
        }
    }
}

void remove_job(pid_t pid) {
    if (pid <= 0) return;

    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].pid == pid) {
            job_table[i].pid = 0;
            job_table[i].id = 0;
            job_table[i].state = 0;
            job_table[i].command[0] = '\0';
            return;
        }
    }
}
