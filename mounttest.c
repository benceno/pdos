#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int
main(int argc, char *argv[])
{
  printf(1, "Mounttests starting\n");
  printmounts();
  mkdir("a");
  mount("README", "a");
  printmounts();
  umount("a");
  printmounts();
  return 0;
}