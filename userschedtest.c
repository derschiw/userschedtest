#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <signal.h>

void print_stack_trace() {
    void *buffer[100];
    int size = backtrace(buffer, 100);  // Capture up to 100 stack frames
    char **symbols = backtrace_symbols(buffer, size);  // Get human-readable symbols

    if (symbols == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    // Print the stack trace
    for (int i = 0; i < size; i++) {
        printf("%s\n", symbols[i]);
    }

    free(symbols);  // Don't forget to free the symbols array
}


void run_dummy_process() {
    while (1) {
        printf("Running dummy process with PID %d\n", getpid());
        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <scheduling_policy>\n", argv[0]);
        fprintf(stderr, "Scheduling policies: 0 = SCHED_OTHER, 1 = SCHED_FIFO, 2 = SCHED_RR, 7 = SCHED_USER\n");
        exit(EXIT_FAILURE);
    }

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
        param.sched_priority = (sched_policy == SCHED_OTHER) ? 0 : 50; // Priority required for FIFO and RR

        if (sched_setscheduler(getpid(), sched_policy, &param) == -1) {
            perror("sched_setscheduler failed");
            print_stack_trace();
            exit(EXIT_FAILURE);
        }
        run_dummy_process();
    } else { // Parent process
        printf("Started child process with PID %d using scheduling policy %d\n", pid, sched_policy);
        wait(NULL);
    }

    return 0;
}
