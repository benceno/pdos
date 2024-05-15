
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"


struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

int
cpuid() {
  return mycpu() - cpus;
}

struct cpu*
mycpu(void)
{
  int apicid, i;

  if (readeflags() & FL_IF)
    panic("mycpu called with interrupts enabled\n");

  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->timeslice_tracker = 0;
  p->ctime = ticks;
  p->stime = 0;
  p->retime = 0;
  p->rutime = 0;
  p->priority = 2;

  release(&ptable.lock);

  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  if((np = allocproc()) == 0){
    return -1;
  }

  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  wakeup1(curproc->parent);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    sleep(curproc, &ptable.lock);
  }
}

void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  for(;;){
    sti();
    acquire(&ptable.lock);

    // Implementação do escalonamento
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Reduz a prioridade dos processos que já utilizaram todo o seu timeslice
      if (p->timeslice_tracker >= INTERV) {
        p->priority++;
        p->timeslice_tracker = 0;
      }

      // Insere o processo na fila de acordo com sua prioridade
      insert_ordered(p);

      p->timeslice_tracker++;
    }

    // Seleciona o próximo processo a ser executado
    p = nextproc();

    if(p != 0){
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      c->proc = 0;
    }

    release(&ptable.lock);
  }
}

// Insere um processo na fila de acordo com a ordem de cada política de escalonamento
void
insert_ordered(struct proc *p)
{
  struct proc *q;
  int inserted = 0;

  // Inserção ordenada
  for(q = ptable.proc; q < &ptable.proc[NPROC]; q++){
    if(q->state != RUNNABLE || q->priority < p->priority){
      memmove(q + 1, q, (int)(&ptable.proc[NPROC] - q) * sizeof(struct proc));
      *q = *p;
      inserted = 1;
      break;
    }
  }

  if (!inserted) {
    *(ptable.proc + NPROC - 1) = *p;
  }
}

// Retorna o próximo processo a ser executado
struct proc*
nextproc(void)
{
  struct proc *p;

  // Seleciona o primeiro processo da fila (FIFO)
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == RUNNABLE){
      return p;
    }
  }

  return 0;
}

void
yield(void)
{
  acquire(&ptable.lock);
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();

  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup locks ptable.lock), so it's okay
  // to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

void
wakeup(void *chan)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
  release(&ptable.lock);
}

void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

void
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return;
    }
  }
  release(&ptable.lock);
}

void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

void
clock(void)
{
  struct proc *p;
  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    switch(p->state){
    case RUNNABLE:
      p->retime++;
      break;
    case RUNNING:
      p->rutime++;
      break;
    case SLEEPING:
      p->stime++;
      break;
    }
  }

  release(&ptable.lock);
}

// Retorna o tempo total do processo p (retime + rutime + stime)
int
get_total_time(struct proc *p)
{
  return p->retime + p->rutime + p->stime;
}

// Retorna o tempo de turnaround do processo p (tempo total - tempo de criação)
int
get_turnaround_time(struct proc *p)
{
  return get_total_time(p) - (ticks - p->ctime);
}

// Retorna o tempo de espera do processo p (tempo total - tempo de execução - tempo de criação)
int
get_waiting_time(struct proc *p)
{
  return get_turnaround_time(p) - p->rutime;
}

// Retorna o tempo de resposta do processo p (tempo de execução - tempo de criação)
int
get_response_time(struct proc *p)
{
  return p->rutime - p->ctime;
}

int set_prio(int priority)
{
  acquire(&ptable.lock);

  if (myproc()->killed || priority < 1 || priority > 3)
  {
    return -1;
  }

  release(&ptable.lock);

  myproc()->priority = priority;

  return 0;
}

int wait2(int *retime, int *rutime, int *stime) {
  struct proc *p;

  int pid;
  int havekids = 0; // Variável para verificar se existem filhos

  acquire(&ptable.lock); // Adquire o lock da tabela de processos

  for (;;) { // Loop infinito para aguardar até que um processo filho termine
    havekids = 0;
    // Percorre a lista de processos
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      // Verifica se o processo atual é pai do processo em análise
      if (p->parent != myproc()) {
        continue; // Se não for pai, continua para o próximo processo
      }
      havekids = 1; // Marca que há pelo menos um filho
      // Se o processo filho terminou (estado ZOMBIE)
      if (p->state == ZOMBIE) {
        // Coleta estatísticas do processo filho
        *stime = p->stime;
        *retime = p->retime;
        *rutime = p->rutime;
        pid = p->pid; // Obtém o PID do processo filho
        // Libera recursos associados ao processo filho
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->state = UNUSED; // Marca o estado como UNUSED
        p->pid = 0; // Reseta o PID
        p->parent = 0; // Reseta o pai
        p->name[0] = 0; // Reseta o nome
        p->killed = 0; // Reseta a flag de kill
        p->ctime = 0; // Reseta o tempo de criação
        p->stime = 0; // Reseta o tempo em estado de espera
        p->retime = 0; // Reseta o tempo de execução
        p->rutime = 0; // Reseta o tempo em estado de execução
        release(&ptable.lock); // Libera o lock da tabela de processos
        return pid; // Retorna o PID do processo filho
      }
    }
      
  // Se não houver processos filhos ou se o processo atual foi sinalizado para terminar
  if (!havekids || myproc()->killed) {
      release(&ptable.lock); // Libera o lock da tabela de processos
      return -1; // Retorna -1 indicando que não há processos filhos para esperar
  }
  
  sleep(myproc(), &ptable.lock); // Aguarda até que um processo filho termine
  }
}
