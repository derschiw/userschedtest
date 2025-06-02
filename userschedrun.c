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
            measure_user(users[i], cmd, 0);
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
            measure_user(users[i],  "head -c 100000000 </dev/urandom | sha256sum > /dev/null", 0);
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
        "head -c 105000000 </dev/urandom | sha256sum > /dev/null",
        "head -c 100000000 </dev/urandom | md5sum > /dev/null",
        "dd if=/dev/urandom of=/dev/null bs=1M count=1000",
        "awk \"BEGIN {for(i=0;i<1300000;i++) x=x+i}\"",
        "yes | head -c 50000000 > /dev/null",
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
                measure_user("root", cmds[i], j);
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

// Now do the same as in test_03 but with differnt number of run per task 
// This will test if something changes after time when the user runs the same command multiple times
void test_04() {
    const char* cmds[] = {
        "head -c 105000000 </dev/urandom | sha256sum > /dev/null",
        "head -c 100000000 </dev/urandom | md5sum > /dev/null",
        "dd if=/dev/urandom of=/dev/null bs=1M count=1000",
        "awk \"BEGIN {for(i=0;i<1300000;i++) x=x+i}\"",
        "yes | head -c 50000000 > /dev/null",
    };
    const int num_cmds = sizeof(cmds) / sizeof(cmds[0]);
    pid_t pids[num_cmds];
    int num_iterations[] = {1, 2, 3, 4, 5}; // Different number of iterations for each command
    const int num_iterations_count = sizeof(num_iterations) / sizeof(num_iterations[0]);
    
    // do this later..

}

// Unqeual number of processes for single user
void test_05() {
    const char* cmds[] = {
        "head -c 10500000 </dev/urandom | sha256sum > /dev/null",
        "head -c 10000000 </dev/urandom | md5sum > /dev/null",
        "dd if=/dev/urandom of=/dev/null bs=1M count=100",
        "dd if=/dev/urandom of=/dev/null bs=1M count=100",
        "dd if=/dev/urandom of=/dev/null bs=1M count=100",
        "dd if=/dev/urandom of=/dev/null bs=1M count=100",
        "dd if=/dev/urandom of=/dev/null bs=1M count=100",
        "dd if=/dev/urandom of=/dev/null bs=1M count=100",
        "awk \"BEGIN {for(i=0;i<130000;i++) x=x+i}\"",
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
                measure_user("root", cmds[i], j);
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
    // Now run in normal scheduling policy for comparison
    printf("Running in normal scheduling policy...\n");
    for (int i = 0; i < num_cmds; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process
            measure_normal("root", cmds[i]);
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

void measure_user(char *usr, char *cmd, int *iteration){
    measure(usr, cmd, iteration, 7);
}
void measure_normal(char *usr, char *cmd, int *iteration){
    measure(usr, cmd, iteration, 1);
}

// This fution will do the actual measurement 
void measure(char *usr, char *cmd, int *iteration, int sched_policy) {
    struct rusage usage_before, usage_after;
    struct timespec start, end;
    char command[512];

    // Measure the time the function has to execute and get resource usage stats
    // Find docs here: https://www.man7.org/linux/man-pages/man2/getrusage.2.html
    getrusage(RUSAGE_SELF, &usage_before);
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // This is the command actually executed
    // It will run the command as the specified user and set the scheduling policy to SCHED_USER (7)
    snprintf(command, sizeof(command), "su - %s -c 'chpol %i %s > /dev/null 2>&1'", usr, sched_policy, cmd);
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

    // Print the results
    printf("%i, %ld, %ld, %ld , %ld, %ld, %s, %s\n",
           iteration,
           ns, utime_ns, stime_ns,
           usage_after.ru_nvcsw - usage_before.ru_nvcsw,
           usage_after.ru_nivcsw - usage_before.ru_nivcsw,
           usr, cmd);
    print_progress(ns, cmd);
}

// Print in color the progress of the command execution
unsigned long hash_str(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void print_progress(long ns, char *cmd){
    int colors[] = {31, 32, 33, 34, 35, 36, 91, 92};
    int num_colors = sizeof(colors) / sizeof(colors[0]);

    unsigned long h = hash_str(cmd);
    int color_code = colors[h % num_colors];
    printf("\033[%dm", color_code);
    for (long i = 0; i < ns / 1000000000 * 2; ++i) {printf("#");}
    printf("\033[0m");
    printf("\n");

}


int main(int argc, char *argv[]) {
    printf("Starting test...\n");
    printf("Iteration, Elapsed time [ns], User CPU time [ns], System CPU time [ns], Voluntary context switches, Involuntary context switches, User, Command\n");
    if (argc < 2) {
        printf("Usage: userschedrun <test_number>\n");
        return 1;
    }
    int test_number = atoi(argv[1]);
    switch (test_number) {
        case 0:
            test_00();
            break;
        case 1:
            test_01();
            break;
        case 2:
            test_02();
            break;
        case 3:
            test_03();
            break;
        case 4:
            test_04();
            break;
        case 5:
            test_05();
            break;
        default:
            printf("Invalid test number. Please use 0, 1, 2, 3, or 4.\n");
            return 1;
    }
    printf("Test completed.\n");
    return 0;
}
