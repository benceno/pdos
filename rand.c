#include "rand.h"
#include "user.h"

int random() {
  static int rand_index;
  int rand_num;
  
  rand_num = rand_array[rand_index];
  rand_index += 1;
  if (rand_index >= SIZE)
    rand_index = 0;

  return rand_num;
}