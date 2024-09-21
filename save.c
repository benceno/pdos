#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];

void
save(int fd, int argc, char *argv[])
/* {
  int n;

  if (write(fd, msg, strlen(msg)) != strlen(msg)) {
    printf(1, "save: write error\n");
    exit();
  }
} */
{
  //int i;
  if ((fd = open(argv[1], O_RDWR | _O_CREATE)) < 0)
  {
    printf(1, "save: cannot open %s\n", argv[1]);
    exit();
  }
  write(fd, argv[2], strlen(argv[2]));
  printf(1, "%s", argv[2]);
  
  /* for(i = 2; i < argc; i++)
  {
    //write(fd, argv[i], strlen(argv[i]));
    printf(1, "%s", argv[i]);
  } */
  //exit();


  /* int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      printf(1, "cat: write error\n");
      exit();
    }
  }
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  } */
}

int main(int argc, char *argv[])
{
  int fd;

  if(argc <= 1)
  {
    printf(1, "save: need 2 arguments\n");
    exit();
  }

  if((fd = open(argv[1], O_RDWR | _O_CREATE)) < 0)
  {
    printf(1, "save: cannot open %s\n", argv[1]);
    exit();
  }
  else
  {
    save(fd, argc, argv);
    close(fd);
  }

  exit();
}
