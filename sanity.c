#include "types.h"
#include "user.h"
#define CPU_BOUND 0
#define S_BOUND 1
#define IO_BOUND 2

void CPU_Bound (void) {
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 1000000; j++){}
  }
}

void S_Bound (void) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 1000000; j++){}
    yield();
  }
}

void IO_Bound (void) {
  for (int i = 0; i < 100; i++) {
    sleep(1);
  }
}

void Create_Process (int type) {
  switch (type) {
    case CPU_BOUND:
      CPU_Bound ();
      break;
    
    case S_BOUND:
      S_Bound ();
      break;

    case IO_BOUND:
      IO_Bound ();
      break;
      
    default:
      break;
  }
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    printf (1, "One parameter expected\n");
  }

  int num_processes = atoi(argv[1]) * 3;
  //int sum_ready_time[3] = {0,0,0};
  //int sum_sleep_time[3] = {0,0,0};
  //int sum_turnaround_time[3] = {0,0,0};

  for (int i = 0; i < num_processes; i++) {
    int process_id = fork();
    if (process_id == 0) {
      Create_Process (getpid() % 3);
      exit ();
    }
  }
  
  for (int i = 0; i < num_processes; i++) {
    wait();
  }

  exit();
}