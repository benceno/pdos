#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE     1000  // size of file system in blocks
#define INTERV        5  // time slice in clock cycles

#define P1TO2       200  // time waiting before promotion from priority 1 to 2 (testar com 5)
#define P2TO3       100  // time waiting before promotion from priority 2 to 3 (testar com 5)
#define P3TO4        50  // time waiting before promotion from priority 3 to 4 (testar com 5)

#define P1TO2       200  // time waiting before promotion from priority 1 to 2
#define P2TO3       100  // time waiting before promotion from priority 2 to 3
#define P3TO4        50  // time waiting before promotion from priority 3 to 4
