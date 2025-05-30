#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Test some infinitely running processes that will heat up the CPU 
void test_00(){
    system("su - root  -c 'chpol 7 yes > /dev/null &'");
    system("su - user1 -c 'chpol 7 yes > /dev/null &'");
    system("su - user2 -c 'chpol 7 yes > /dev/null &'");
    system("su - user3 -c 'chpol 7 yes > /dev/null &'");
    return 0;
}

// This will burn CPU but the workload is the same for all users
void test_01(){
    system("su - root  -c \"chpol 7 awk 'BEGIN{for(i=0;i<1e8;i++)x+=i} END{print x}' > /dev/null\"");
    system("su - user1 -c \"chpol 7 awk 'BEGIN{for(i=0;i<1e8;i++)x+=i} END{print x}' > /dev/null\"");
    system("su - user2 -c \"chpol 7 awk 'BEGIN{for(i=0;i<1e8;i++)x+=i} END{print x}' > /dev/null\"");
    system("su - user3 -c \"chpol 7 awk 'BEGIN{for(i=0;i<1e8;i++)x+=i} END{print x}' > /dev/null\"");
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
    measure(test_01);
    printf("Test completed.\n");
    return 0;
}
