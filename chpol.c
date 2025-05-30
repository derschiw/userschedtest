#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <scheduling_policy> <command> [args...]\n", argv[0]);
        fprintf(stderr, "Scheduling policies: 0 = SCHED_OTHER, 1 = SCHED_FIFO, 2 = SCHED_RR, 7 = SCHED_USER\n");
        exit(EXIT_FAILURE);
    }

    int SCHED_USER = 7;
    int input_policy = atoi(argv[1]);
    int sched_policy;

    switch (input_policy) {
        case 0:
            sched_policy = SCHED_OTHER;
            break;
        case 1:
            sched_policy = SCHED_FIFO;
            break;
        case 2:
            sched_policy = SCHED_RR;
            break;
        case 7:
            sched_policy = SCHED_USER;
            break;
        default:
            fprintf(stderr, "Invalid scheduling policy: %d.\n", input_policy);
            exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { 
        struct sched_param param;
        param.sched_priority = (sched_policy == SCHED_OTHER || sched_policy == SCHED_USER) ? 0 : 50;

        if (sched_setscheduler(getpid(), sched_policy, &param) == -1) {
            perror("sched_setscheduler failed");
            exit(EXIT_FAILURE);
        }

        // Execute given command with arguments 
        execvp(argv[2], &argv[2]);

        // If execvp fails
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        printf("Started child process with PID %d using scheduling policy %d\n", pid, sched_policy);
        wait(NULL);
    }

    return 0;
}