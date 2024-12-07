#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "strace.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_strace(void)
{
  int enable;
  if(argint(0, &enable) < 0)
    return -1;
  strace_enabled = enable;
  return 0;
}

int
sys_stracedump(void)
{
  stracedump();
  return 0;
}

int
sys_strace_filter(void)
{
    char *filter;
    if(argstr(0, &filter) < 0)
        return -1;
    
    safestrcpy(trace_flags.syscall_filter, filter, sizeof(trace_flags.syscall_filter));
    trace_flags.one_time_filter = 1;
    
    return 0;
}

int
sys_strace_success(void)
{
    int enable;
    if(argint(0, &enable) < 0)
        return -1;
    trace_flags.success_only = enable;
    trace_flags.one_time_filter = 1;
    return 0;
}

int
sys_strace_fail(void)
{
    int enable;
    if(argint(0, &enable) < 0)
        return -1;
    trace_flags.fail_only = enable;
    trace_flags.one_time_filter = 1;
    return 0;
}