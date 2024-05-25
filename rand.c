#include "rand.h"

int random() {
  int rand_num;
  
  rand_num = rand_array[rand_index];
  rand_index += 1;
  if (rand_index >= SIZE)
    rand_index = 0;

  return rand_num;
}