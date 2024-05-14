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
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf(1, "Invalid number of arguments passed\n");
        exit();
    }
    int numberProcesses = atoi(argv[1]);
    int amount = numberProcesses * 3;

    for (int i = 0; i < amount; i++)
    {
        int pid = fork();
        if (pid == -1)
        {
            continue;
        }
        enum proctype type = i % 3;
        switch (type)
        {
        case CPU_BOUND:
            for (int j = 0; j < 100; j++)
            {
                empty_loop(10000);
            }
            break;
        case S_BOUND:
            for (int j = 0; j < 20; j++)
            {
                empty_loop(10000);
            }
            break;
        case IO_BOUND:
            for (int j = 0; j < 100; j++)
            {
                sleep(1);
            }
            break;
        default:
            break;
        }
        int *retime = malloc(sizeof(int *)), *rutime = malloc(sizeof(int *)), *stime = malloc(sizeof(int *));
        wait2(retime, rutime, stime);
        printf(1, "retime: %d, rutime: %d, stime: %d\n", *retime, *rutime, *stime);
    }
    return 0;
}