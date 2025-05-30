#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/resource.h>

// Test some infinitely running processes that will heat up the CPU 
void test_00(){
    system("su - root  -c 'chpol 7 yes > /dev/null &'");
    system("su - user1 -c 'chpol 7 yes > /dev/null &'");
    system("su - user2 -c 'chpol 7 yes > /dev/null &'");
    system("su - user3 -c 'chpol 7 yes > /dev/null &'");
    return 0;
}

// This will burn CPU but the workload is the same for all users
// This will read 100 million bytes of random data from /dev/urandom and compute its SHA256 hash.
// head -c 10000000 </dev/urandom | sha256sum > /dev/null
// This is the same workload for each task => good for measuring the scheduler performance

void test_01() {
    const char* users[] = {"root", "user1", "user2", "user3"};
    const int num_users = sizeof(users) / sizeof(users[0]);
    pid_t pids[num_users];

    for (int i = 0; i < num_users; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "head -c 100000000 </dev/urandom | sha256sum > /dev/null");
            measure(users[i], cmd);
            exit(0);
        } else if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }
    }

    // Wait childs processes to finish
    for (int i = 0; i < num_users; ++i) {
        waitpid(pids[i], NULL, 0);
    }
}

// increase the number of processes to 8 for each user (cmp to test_01)
void test_02() {
    const char* users[] = {"root", "user1", "user2", "user3","root", "user1", "user2", "user3"};
    const int num_users = sizeof(users) / sizeof(users[0]);
    pid_t pids[num_users];

    for (int i = 0; i < num_users; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process
            measure(users[i],  "head -c 100000000 </dev/urandom | sha256sum > /dev/null");
            exit(0);
        } else if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }
    }

    // Wait childs processes to finish
    for (int i = 0; i < num_users; ++i) {
        waitpid(pids[i], NULL, 0);
    }
}


// A bunch of useless commands that will keep the CPU busy. Sorry climate..
void test_03() {
    const char* cmds[] = {
        "head -c 100000000 </dev/urandom | sha256sum > /dev/null",
        "head -c 100000000 </dev/urandom | md5sum > /dev/null",
        "dd if=/dev/urandom of=/dev/null bs=1M count=1000",
        "awk \"BEGIN {for(i=0;i<1000000;i++) x=x+i}\"",
        "yes | head -c 10000000 > /dev/null",
    };
    const int num_cmds = sizeof(cmds) / sizeof(cmds[0]);
    pid_t pids[num_cmds];
    int num_iterations = 3; // Number of iterations for each command

    for (int j = 0; j < num_iterations; ++j) {
        // Fork processes for each command
        for (int i = 0; i < num_cmds; ++i) {
            pids[i] = fork();
            if (pids[i] == 0) {
                // Child process
                measure("root", cmds[i]);
                exit(0);
            } else if (pids[i] < 0) {
                perror("fork failed");
                exit(1);
            }
        }
        
        // Wait childs processes to finish
        for (int i = 0; i < num_cmds; ++i) {
            waitpid(pids[i], NULL, 0);
        }
    }
}


// This fution will do the actual measurement 
void measure(char *usr, char *cmd) {
    struct rusage usage_before, usage_after;
    struct timespec start, end;
    char command[512];

    // Measure the time the function has to execute and get resource usage stats
    // Find docs here: https://www.man7.org/linux/man-pages/man2/getrusage.2.html
    getrusage(RUSAGE_SELF, &usage_before);
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // This is the command actually executed
    // It will run the command as the specified user and set the scheduling policy to SCHED_USER (7)
    snprintf(command, sizeof(command), "su - %s -c 'chpol 7 %s'", usr, cmd);
    int result = system(command);
    
    // Finisk the measurement
    clock_gettime(CLOCK_MONOTONIC, &end);
    getrusage(RUSAGE_SELF, &usage_after);


    // Conversions and resource extractions 
    long ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    long utime_ns = (usage_after.ru_utime.tv_sec - usage_before.ru_utime.tv_sec) * 1000000000 +
                    (usage_after.ru_utime.tv_usec - usage_before.ru_utime.tv_usec) * 1000;
    long stime_ns = (usage_after.ru_stime.tv_sec - usage_before.ru_stime.tv_sec) * 1000000000 +
                    (usage_after.ru_stime.tv_usec - usage_before.ru_stime.tv_usec) * 1000;

    printf("Command executed: %s\n", command); fflush(stdout);
    printf("Command executed as: %s\n", usr); fflush(stdout);
    printf("Elapsed time: %ld ns\n", ns); fflush(stdout);
    printf("User CPU time: %ld ns\n", utime_ns); fflush(stdout);
    printf("System CPU time: %ld ns\n", stime_ns); fflush(stdout);
    printf("Number of voluntary context switches: %ld\n", usage_after.ru_nvcsw - usage_before.ru_nvcsw); fflush(stdout);
    printf("Number of involuntary context switches: %ld\n", usage_after.ru_nivcsw - usage_before.ru_nivcsw); fflush(stdout);
    printf("\n");
}

int main() {
    printf("Starting test...\n");
    test_03();
    printf("Test completed.\n");
    return 0;
}
