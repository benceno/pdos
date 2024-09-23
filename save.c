#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

void
save(int fd, int argc, char *argv[])
{
  int i;
  int n;

  // prevent overwriting the file
  n = read(fd, buf, sizeof(buf));

  // add a newline if the file is not empty
  if(n > 0)
  {
    write(fd, "\n", 1);
  }

  // write new data to the file
  for(i = 2; i < argc; i++)
  {
    write(fd, argv[i], strlen(argv[i]));
    write(fd, " ", 1);
  }
}

int main(int argc, char *argv[])
{
  int fd;

  if(argc <= 1)
  {
    printf(2, "save: need 2 arguments\n");
    exit();
  }

  if((fd = open(argv[1], O_RDWR | O_CREATE)) < 0)
  {
    printf(2, "save: cannot open %s\n", argv[1]);
    exit();
  }
  else
  {
    save(fd, argc, argv);
    close(fd);
  }
  exit();
}
