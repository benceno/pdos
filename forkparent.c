#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
  const int n = 10;

  for (int i = 0; i < n; i++) {
    const int pid = fork();
    if (pid == 0) {
      printf(1, "Child process: PID = %d, PPID = %d\n", getpid(), getppid());
      exit();
    } else if (pid > 0) {
    } else {
      printf(1, "Fork failed\n");
    }
  }

  for (int i = 0; i < n; i++) {
    wait();
  }

  exit();
}