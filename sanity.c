#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
void empty_loop(int n)
{
    for (int i = 0; i < n; i++)
    {
        ;
    }
}
enum proctype
{
    CPU_BOUND,
    S_BOUND,
    IO_BOUND
};

char *proctype_string[] = {
    "CPU_BOUND",
    "S_BOUND",
    "IO_BOUND"};

void io_bound_loop()
{
    for (int j = 0; j < 100; j++)
    {
        sleep(1);
    }
    exit();
}
void s_bound_loop()
{
    for (int j = 0; j < 20; j++)
    {
        empty_loop(1000000);
        yield();
    }
    exit();
}
void cpu_bound_loop()
{
    for (int j = 0; j < 100; j++)
    {
        empty_loop(1000000);
    }
    exit();
}

void run_bound_loop(enum proctype type)
{
    switch (type)
    {
    case CPU_BOUND:
        cpu_bound_loop();
    case S_BOUND:
        s_bound_loop();
    case IO_BOUND:
        io_bound_loop();
    default:
        break;
    }
}
typedef struct process_info
{
    int pid;
    enum proctype type;
} process_info;

int get_process_index(int pid, process_info *process_pids, int numberProcesses)
{
    for (int i = 0; i < numberProcesses; i++)
    {
        if (process_pids[i].pid == pid)
        {
            return i;
        }
    }
    return -1;
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf(1, "Invalid number of arguments passed\n");
        exit();
    }

    int amount = atoi(argv[1]);
    int numberProcesses = amount*3;

    int retime, rutime, stime;
    int total[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

    process_info *process_pids = malloc(numberProcesses * sizeof(process_info));
    process_info process_info;
    int processCreated = 0;
    for (int i = 0; i < numberProcesses; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            printf(1, "fork failed\n");
            break;
        }
        processCreated++;
        enum proctype type = i % 3;
        if (pid)
        {
            process_info.pid = pid;
            process_info.type = type;
            process_pids[i] = process_info;
            continue;
        }
        run_bound_loop(type);
    }
    for (int i = 0; i < processCreated; i++)
    {
        int wait_pid = wait2(&retime, &rutime, &stime);
        int process_index = get_process_index(wait_pid, process_pids, numberProcesses);
        process_info = process_pids[process_index];
        enum proctype type = process_info.type;
        printf(1, "pid: %d, type: %s, retime: %d, rutime: %d, stime: %d\n", process_info.pid, proctype_string[type], retime, rutime, stime);
        total[type][0] += retime;
        total[type][1] += rutime;
        total[type][2] += stime + rutime + retime;
    }
    printf(1, "Process averages\n");
    for (int i = 0; i < 3; i++)
    {
        printf(1, "type: %s, retime: %d, rutime: %d, turnaround: %d\n", proctype_string[i],
               total[i][0] / processCreated,
               total[i][1] / processCreated,
               total[i][2] / processCreated);
    }
    exit();
    return 0;
}
