#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <signal.h>

void run_dummy_process() {
    printf("Running dummy process with PID %d\n", getpid());
    while (1) {
        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <scheduling_policy>\n", argv[0]);
        fprintf(stderr, "Scheduling policies: 0 = SCHED_OTHER, 1 = SCHED_FIFO, 2 = SCHED_RR, 7 = SCHED_USER\n");
        exit(EXIT_FAILURE);
    }

    /* As building happens on a local machine - it doesnt have SCHED_USER in sched.h */
    int SCHED_USER = 7; // Define SCHED_USER for compatibility
    int policy;
    policy = atoi(argv[1]);
    int sched_policy;

    switch (policy) {
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
            fprintf(stderr, "Invalid scheduling policy. Use 0, 1, 2 or 7.\n");
            exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) { // Child process
        struct sched_param param;
        param.sched_priority = (sched_policy == SCHED_OTHER || sched_policy == SCHED_USER) ? 0 : 50; // Priority required for FIFO and RR

        if (sched_setscheduler(getpid(), sched_policy, &param) == -1) {
            perror("sched_setscheduler failed");
            exit(EXIT_FAILURE);
        }
        run_dummy_process();
    } else { // Parent process
        printf("Started child process with PID %d using scheduling policy %d\n", pid, sched_policy);
        wait(NULL);
    }

    return 0;
}
