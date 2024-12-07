#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "strace.h"


int strace_enabled = 0;
struct trace_flags trace_flags; 

struct {
  struct trace_event events[MAX_TRACE_EVENTS];
  int next_index;
  int count;
} trace_buffer;

void
init_trace_buffer(void)
{
  trace_buffer.next_index = 0;
  trace_buffer.count = 0;
  int i;
  for(i = 0; i < MAX_TRACE_EVENTS; i++) {
    trace_buffer.events[i].valid = 0;
  }
}

void
add_trace_event(int pid, const char *cmd_name, const char *sys_name, int ret_val)
{
  struct trace_event *event = &trace_buffer.events[trace_buffer.next_index];
  
  event->pid = pid;
  strncpy(event->command_name, cmd_name, 16);
  strncpy(event->syscall_name, sys_name, 20);
  event->return_value = ret_val;
  event->valid = 1;
  
  trace_buffer.next_index = (trace_buffer.next_index + 1) % MAX_TRACE_EVENTS;
  if(trace_buffer.count < MAX_TRACE_EVENTS)
    trace_buffer.count++;
}



// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_strace(void);
extern int sys_stracedump(void); 
extern int sys_strace_filter(void);
extern int sys_strace_success(void);
extern int sys_strace_fail(void);


static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_strace] sys_strace,
[SYS_stracedump]   sys_stracedump,
[SYS_strace_filter] sys_strace_filter,
[SYS_strace_success] sys_strace_success,
[SYS_strace_fail]    sys_strace_fail,
};

static const char *syscall_names[] = {
  [SYS_fork]    "fork",
  [SYS_exit]    "exit",
  [SYS_wait]    "wait",
  [SYS_pipe]    "pipe",
  [SYS_read]    "read",
  [SYS_kill]    "kill",
  [SYS_exec]    "exec",
  [SYS_fstat]   "fstat",
  [SYS_chdir]   "chdir",
  [SYS_dup]     "dup",
  [SYS_getpid]  "getpid",
  [SYS_sbrk]    "sbrk",
  [SYS_sleep]   "sleep",
  [SYS_uptime]  "uptime",
  [SYS_open]    "open",
  [SYS_write]   "write",
  [SYS_mknod]   "mknod",
  [SYS_unlink]  "unlink",
  [SYS_link]    "link",
  [SYS_mkdir]   "mkdir",
  [SYS_close]   "close",
  [SYS_strace]  "trace",
  [SYS_stracedump]   "dump",
  [SYS_strace_filter] "strace_filter",
  [SYS_strace_success] "strace_success",
  [SYS_strace_fail] "strace_fail",
};


// void
// syscall(void)
// {
//   int num;
//   struct proc *curproc = myproc();

//   num = curproc->tf->eax;
//   if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
//     int ret = syscalls[num]();
//     curproc->tf->eax = ret;
    
//     if(strace_enabled) {
//       cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n", 
//              curproc->pid, curproc->name, syscall_names[num], ret);
//     }
//   } else {
//     cprintf("%d %s: unknown sys call %d\n",
//             curproc->pid, curproc->name, num);
//     curproc->tf->eax = -1;
//   }
// }

void
syscall(void)
{
    int num;
    struct proc *curproc = myproc();

    num = curproc->tf->eax;
    if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {

        // handle exit sys calls
        if(num == SYS_exit && strace_enabled && 
           strncmp(curproc->name, "sh", 2) != 0 && 
           strncmp(curproc->name, "strace", 6) != 0) {
            
            // For regular strace with no filters
            // store and print exit
            if(trace_flags.syscall_filter[0] == '\0' && 
               !trace_flags.success_only && !trace_flags.fail_only) {
                add_trace_event(curproc->pid, curproc->name, syscall_names[num], 0);
                cprintf("TRACE: pid = %d | command_name = %s | syscall = exit\n",
                       curproc->pid, curproc->name);
            }
            
            // clear filters after process exits if using filters
            if(trace_flags.one_time_filter) {
                trace_flags.syscall_filter[0] = '\0';
                trace_flags.success_only = 0;
                trace_flags.fail_only = 0;
                trace_flags.one_time_filter = 0;
            }
            
            int ret = syscalls[num]();
            curproc->tf->eax = ret;
            return;
        }
        
        // for write syscalls during tracing, don't execute them for better readabilytu 
        if(num == SYS_write && strace_enabled && 
           strncmp(curproc->name, "sh", 2) != 0 && 
           strncmp(curproc->name, "strace", 6) != 0) {
            curproc->tf->eax = 1;  // Pretend write succeeded
        } else {
            // For all other syscalls, execute normally
            int ret = syscalls[num]();
            curproc->tf->eax = ret;
        }
        
        if(strace_enabled && 
           strncmp(curproc->name, "sh", 2) != 0 && 
           strncmp(curproc->name, "strace", 6) != 0) {
            
            // determine if trace should be printed 
            int should_print = 1;
            int ret = curproc->tf->eax;  // get return value
            
            // check syscall name filter if active
            if(trace_flags.syscall_filter[0] != '\0') {
                should_print = (strncmp(trace_flags.syscall_filter, syscall_names[num], 
                              strlen(trace_flags.syscall_filter)) == 0);
            }
            
            // check success/fail filters
            if(should_print && trace_flags.success_only)
                should_print = (ret >= 0);
            if(should_print && trace_flags.fail_only)
                should_print = (ret < 0);
            
            // store and print only if passed all filters
            if(should_print) {
                add_trace_event(curproc->pid, curproc->name, syscall_names[num], ret);
                cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                       curproc->pid, curproc->name, syscall_names[num], ret);
            }
        }
    } else {
        cprintf("%d %s: unknown sys call %d\n",
                curproc->pid, curproc->name, num);
        curproc->tf->eax = -1;
    }
}



void
stracedump(void)
{
  int i;
  int start = (trace_buffer.next_index - trace_buffer.count + MAX_TRACE_EVENTS) % MAX_TRACE_EVENTS;
  
  // disable tracing while dumping
  int old_strace = strace_enabled;
  strace_enabled = 0;
  
  for(i = 0; i < trace_buffer.count; i++) {
    int idx = (start + i) % MAX_TRACE_EVENTS;
    struct trace_event *event = &trace_buffer.events[idx];
    if(event->valid) {
      if(strncmp(event->syscall_name, "exit", 4) == 0)
        cprintf("DUMP: pid = %d | command_name = %s | syscall = %s\n", 
               event->pid, event->command_name, event->syscall_name);
      else
        cprintf("DUMP: pid = %d | command_name = %s | syscall = %s | return value = %d\n", 
               event->pid, event->command_name, event->syscall_name, event->return_value);
    }
  }
  
  // Restore original tracing state
  strace_enabled = old_strace;
}
