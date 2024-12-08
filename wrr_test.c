#include "types.h"
#include "user.h"
#include "fcntl.h"

// Function to simulate creating processes with different priorities
void create_processes(int burst_time, int confidence) {
    int i;

    // Create processes for Priority 1 (RR)
    for (i = 1; i <= 6; i++) {
        if (fork() == 0) {
            // Child process
            setpriority(1);    // Priority 1
            setburst(burst_time); // Arbitrary burst time
            printf(1, "Process PID=%d created in Priority 1 (RR), Burst=%d\n", getpid(), burst_time);
            for (int j = 0; j < 10000000000; j++); // Arbitrary large loop count
            exit(); // Terminate the child process
        }
    }

    // Create processes for Priority 2 (SJF)
    for (i = 1; i <= 6; i++) {
        if (fork() == 0) {
            // Child process
            setpriority(2);       // Priority 2
            setburst(burst_time); // Arbitrary burst time
            setconfidence(confidence); // Confidence value
            printf(1, "Process PID=%d created in Priority 2 (SJF), Burst=%d, Confidence=%d\n", getpid(), burst_time, confidence);
            for (int j = 0; j < 10000000000; j++); // Arbitrary large loop count
            exit(); // Terminate the child process
        }
    }

    // Create processes for Priority 3 (FCFS)
    for (i = 1; i <= 6; i++) {
        if (fork() == 0) {
            // Child process
            setpriority(3);    // Priority 3
            setburst(burst_time); // Arbitrary burst time
            printf(1, "Process PID=%d created in Priority 3 (FCFS), Burst=%d\n", getpid(), burst_time);
            for (int j = 0; j < 10000000000; j++); // Arbitrary large loop count
            exit(); // Terminate the child process
        }
    }
}

// Main function to start the test
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf(1, "Usage: test_scheduler <burst_time> <confidence>\n");
        exit();
    }

    // Parse inputs
    int burst_time = atoi(argv[1]);
    int confidence = atoi(argv[2]);

    // Validate inputs
    if (burst_time <= 0 || confidence < 0 || confidence > 99) {
        printf(1, "Invalid inputs. Burst time must be > 0 and confidence between 0-99.\n");
        exit();
    }

    printf(1, "Starting test for WRR Scheduler with 18 processes\n");

    // Create processes
    create_processes(burst_time, confidence);

    // Wait for a while to observe scheduler behavior
    //sleep(1000); // Sleep for 1000ms (adjust as needed)

    // Exit the parent process
    printf(1, "Test completed.\n");
    exit();
}
