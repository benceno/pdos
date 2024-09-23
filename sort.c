#include "types.h"
#include "stat.h"
#include "user.h"

void swap(int *x, int *y)
{
  int temp = *x;
  *x = *y;
  *y = temp;
}

void selectionSort(int arr[], int n)
{
  int i, j, min_idx;

  for (i = 0; i < n-1; i++)
  {
    min_idx = i;
    for (j = i+1; j < n; j++)
    {
      if(arr[j] < arr[min_idx])
        min_idx = j;
    }
      if(min_idx != i)
      swap(&arr[min_idx], &arr[i]);
  }
}

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
