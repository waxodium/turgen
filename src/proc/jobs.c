#include "jobs.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_JOBS 100


static Job jobs[MAX_JOBS];
static int next_id = 1;


static Job *find_free(void) {
    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].id == 0)
            return &jobs[i];

    return NULL;
}


static Job *find_id(int id) {
    if (id <= 0)
        return NULL;

    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].id == id)
            return &jobs[i];

    return NULL;
}


static Job *find_pid(pid_t pid) {
    if (pid <= 0)
        return NULL;

    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].id != 0 && jobs[i].pid == pid)
            return &jobs[i];

    return NULL;
}


static int get_id(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        int id = next_id++;

        if (next_id <= 0)
            next_id = 1;

        if (id > 0 && !find_id(id))
            return id;
    }

    return -1;
}


void jobs_init(void) {
    memset(jobs, 0, sizeof(jobs));
    next_id = 1;
}


void jobs_clear(void) {
    memset(jobs, 0, sizeof(jobs));
    next_id = 1;
}


Job *job_create(pid_t pid, const char *command) {
    Job *job = find_free();

    if (!job || pid <= 0)
        return NULL;

    int id = get_id();

    if (id < 0)
        return NULL;

    memset(job, 0, sizeof(*job));

    job->id = id;
    job->pid = pid;
    job->pgid = pid;
    job->state = JOB_RUNNING;

    if (command)
        strncpy(job->command, command, sizeof(job->command) - 1);

    return job;
}


Job *job_get(int id) {
    return find_id(id);
}


Job *job_get_pid(pid_t pid) {
    return find_pid(pid);
}


int job_update(pid_t pid, int status) {
    Job *job = find_pid(pid);

    if (!job)
        return -1;

    job->status = status;

    if (WIFSTOPPED(status))
        job->state = JOB_STOPPED;
    else if (WIFCONTINUED(status))
        job->state = JOB_RUNNING;
    else if (WIFSIGNALED(status))
        job->state = JOB_TERMINATED;
    else if (WIFEXITED(status))
        job->state = JOB_DONE;

    return 0;
}


int jobs_reap(void) {
    int count = 0;

    while (1) {
        int status;
        pid_t pid = waitpid(
            -1,
            &status,
            WNOHANG | WUNTRACED | WCONTINUED
        );

        if (pid > 0) {
            job_update(pid, status);
            count++;
            continue;
        }

        if (pid == 0)
            break;

        if (errno == EINTR)
            continue;

        break;
    }

    return count;
}


int job_wait(int id) {
    Job *job = find_id(id);

    if (!job)
        return -1;

    while (job->state == JOB_RUNNING || job->state == JOB_STOPPED) {

        int status;
        pid_t pid;

        do {
            pid = waitpid(job->pid, &status, WUNTRACED);
        } while (pid < 0 && errno == EINTR);

        if (pid < 0)
            return -1;

        job_update(pid, status);
    }

    return 0;
}


int job_kill(int id, int signal) {
    Job *job = find_id(id);

    if (!job)
        return -1;

    return kill(job->pgid, signal);
}


int job_remove(int id) {
    Job *job = find_id(id);

    if (!job)
        return -1;

    memset(job, 0, sizeof(*job));

    return 0;
}


size_t jobs_count(void) {
    size_t count = 0;

    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].id != 0)
            count++;

    return count;
}


Job *jobs_at(size_t index) {
    size_t count = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].id == 0)
            continue;

        if (count == index)
            return &jobs[i];

        count++;
    }

    return NULL;
}


const char *job_state_string(JobState state) {
    switch (state) {
    case JOB_RUNNING:
        return "Running";
    case JOB_STOPPED:
        return "Stopped";
    case JOB_DONE:
        return "Done";
    case JOB_TERMINATED:
        return "Terminated";
    default:
        return "Unknown";
    }
}
