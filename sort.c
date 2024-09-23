#include "sort.h"
#include "types.h"
#include "stat.h"
#include "user.h"

void sort(int argc, char *argv[])
{
  int i;
  int arr[argc-1];

  // convert the arguments to integers
  for(i = 1; i < argc; i++)
  {
    arr[i-1] = atoi(argv[i]);
  }

  // sort the array
  selectionSort(arr, argc-1);

  printf(1, "Sorted array: ");

  // print the sorted array to terminal
  for(i = 0; i < argc-1; i++)
  {
    printf(1, "%d ", arr[i]);
  }

  printf(1, "\n");
}

int main(int argc, char *argv[])
{
  if(argc <= 1)
  {
    printf(1, "sort: needs at least one argument\n");
    exit();
  }

  sort(argc, argv);
  exit();
}
