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
// This will read 10 million bytes of random data from /dev/urandom and compute its SHA256 hash.
// head -c 10000000 </dev/urandom | sha256sum > /dev/null
// This is the same workload for each task => good for measuring the scheduler performance
void test_01(){
    pid_t child1, child2, child3, child4;
    child1 = fork();
    if (child1 == 0) {
        // Child process for root
        measure(system("su - root  -c \"chpol 7 head -c 10000000 </dev/urandom | sha256sum > /dev/null\""));
        exit(0);
    }
    child2 = fork();
    if (child2 == 0) {
        // Child process for user1
        measure(system("su - user1 -c \"chpol 7 head -c 10000000 </dev/urandom | sha256sum > /dev/null\""));
        exit(0);
    }
    child3 = fork();
    if (child3 == 0) {
        // Child process for user2
        measure(system("su - user2 -c \"chpol 7 head -c 10000000 </dev/urandom | sha256sum > /dev/null\""));
        exit(0);
    }
    child4 = fork();
    if (child4 == 0) {
        // Child process for user3
        measure(system("su - user3 -c \"chpol 7 head -c 10000000 </dev/urandom | sha256sum > /dev/null\""));
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
void measure(void (*function)(void)) {
    struct rusage usage_before, usage_after;
    struct timespec start, end;
    long ms;

    // Measure the time the function has to execute and get resource usage stats
    // Find docs here: https://www.man7.org/linux/man-pages/man2/getrusage.2.html
    getrusage(RUSAGE_SELF, &usage_before);
    clock_gettime(CLOCK_MONOTONIC, &start);
    function();
    clock_gettime(CLOCK_MONOTONIC, &end);
    getrusage(RUSAGE_SELF, &usage_after);


    // Conversions and resource extractions 
    ms = (end.tv_sec - start.tv_sec) * 1000 +
         (end.tv_nsec - start.tv_nsec) / 1000000;

    long utime_ms = (usage_after.ru_utime.tv_sec - usage_before.ru_utime.tv_sec) * 1000 +
                    (usage_after.ru_utime.tv_usec - usage_before.ru_utime.tv_usec) / 1000;

    long stime_ms = (usage_after.ru_stime.tv_sec - usage_before.ru_stime.tv_sec) * 1000 +
                    (usage_after.ru_stime.tv_usec - usage_before.ru_stime.tv_usec) / 1000;

    printf("Elapsed time: %ld ms\n", ms);
    printf("User CPU time: %ld ms\n", utime_ms);
    printf("System CPU time: %ld ms\n", stime_ms);
    printf("Number of voluntary context switches: %ld\n", usage_after.ru_nvcsw - usage_before.ru_nvcsw);
    printf("Number of involuntary context switches: %ld\n", usage_after.ru_nivcsw - usage_before.ru_nivcsw);
}

int main() {
    printf("Starting test...\n");
    test_01();
    printf("Test completed.\n");
    return 0;
}
