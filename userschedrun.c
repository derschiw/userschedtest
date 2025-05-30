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
void test_01(){
    pid_t child1, child2, child3, child4;
    child1 = fork();
    if (child1 == 0) {
        // Child process for root
        measure("root", "chpol 7 head -c 100000000 </dev/urandom | sha256sum > /dev/null");
        exit(0);
    }
    child2 = fork();
    if (child2 == 0) {
        // Child process for user1
        measure("user1", "chpol 7 head -c 100000000 </dev/urandom | sha256sum > /dev/null");
        exit(0);
    }
    child3 = fork();
    if (child3 == 0) {
        // Child process for user2
        measure("user2", "chpol 7 head -c 100000000 </dev/urandom | sha256sum > /dev/null");
        exit(0);
    }
    child4 = fork();
    if (child4 == 0) {
        // Child process for user3
        measure("user3", "chpol 7 head -c 100000000 </dev/urandom | sha256sum > /dev/null");
        exit(0);
    }
    // Wait for all child processes to finish
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    waitpid(child3, NULL, 0);  
    waitpid(child4, NULL, 0);
    return 0;
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
    snprintf(command, sizeof(command), "su - %s -c '%s'", usr, cmd);
    int result = system(command);
    clock_gettime(CLOCK_MONOTONIC, &end);
    getrusage(RUSAGE_SELF, &usage_after);


    // Conversions and resource extractions 
    long ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    long utime_ns = (usage_after.ru_utime.tv_sec - usage_before.ru_utime.tv_sec) * 1000000000 +
                    (usage_after.ru_utime.tv_usec - usage_before.ru_utime.tv_usec) * 1000;
    long stime_ns = (usage_after.ru_stime.tv_sec - usage_before.ru_stime.tv_sec) * 1000000000 +
                    (usage_after.ru_stime.tv_usec - usage_before.ru_stime.tv_usec) * 1000;

    printf("Elapsed time: %ld ns\n", ns); fflush(stdout);
    printf("User CPU time: %ld ns\n", utime_ns); fflush(stdout);
    printf("System CPU time: %ld ns\n", stime_ns); fflush(stdout);
    printf("Number of voluntary context switches: %ld\n", usage_after.ru_nvcsw - usage_before.ru_nvcsw); fflush(stdout);
    printf("Number of involuntary context switches: %ld\n", usage_after.ru_nivcsw - usage_before.ru_nivcsw); fflush(stdout);
}

int main() {
    printf("Starting test...\n");
    test_01();
    printf("Test completed.\n");
    return 0;
}
