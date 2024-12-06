#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_PROCESSES 5
#define WORK_ITERATIONS 50

void busy_work(int id) {
  for (int i = 0; i < WORK_ITERATIONS; i++) {
    printf(1, "Process %d: iteration %d\n", id, i);
    // Simulate some work
    for (volatile int j = 0; j < 1000000; j++) {}
  }
  exit();
}

int main(void) {
  int i;
  printf(1, "Testing scheduler with %d processes\n", NUM_PROCESSES);

  for (i = 0; i < NUM_PROCESSES; i++) {
    int pid = fork();
    if (pid < 0) {
      printf(1, "Fork failed\n");
      exit();
    } else if (pid == 0) {
      // Child process: perform busy work
      busy_work(i + 1);
    }
  }

  // Parent process: wait for all children
  for (i = 0; i < NUM_PROCESSES; i++) {
    wait();
  }

  printf(1, "Scheduler test completed\n");
  exit();
}
