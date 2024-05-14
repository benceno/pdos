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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf(1, "Invalid number of arguments passed\n");
        exit();
    }

    int amount = atoi(argv[1]);
    int numberProcesses = amount * 3;

    int retime, rutime, stime;
    int total[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    for (int i = 0; i < numberProcesses; i++)
    {
        int pid = fork();
        enum proctype type = i % 3;
        if (pid)
        {
            if (wait2(&retime, &rutime, &stime) == pid)
            {
                printf(1, "pid: %d, type: %s, retime: %d, rutime: %d, stime: %d\n", i, proctype_string[type], retime, rutime, stime);
                total[type][0] += retime;
                total[type][1] += rutime;
                total[type][2] += stime + rutime + retime;
            }

            continue;
        }
        run_bound_loop(type);
    }

    printf(1, "Process averages\n");
    for (int i = 0; i < 3; i++)
    {
        printf(1, "type: %s, retime: %d, rutime: %d, turnaround: %d\n", proctype_string[i],
               total[i][0] / numberProcesses,
               total[i][1] / numberProcesses,
               total[i][2] / numberProcesses);
    }
    exit();
    return 0;
}
