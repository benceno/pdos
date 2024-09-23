#include "sort.h"
#include "types.h"
#include "user.h"

void advance(char **str, const char c) {
  while (**str && *((*str)++) != c)
    ;
}

int abs(int x) { return x > 0 ? x : -x; }

int sqrt_int(int target, int cur, int a, int b) {
  const int cur2 = cur * cur;
  int diff = target - cur2;
  if (abs(diff) < 100 || a >= b) {
    return cur;
  }
  return diff > 0 ? sqrt_int(target, (b + cur + 1) / 2, cur + 1, b)
                  : sqrt_int(target, (a + cur - 1) / 2, a, cur - 1);
}

int mysqrt(int val) { return sqrt_int(val, val / 2, 0, val); }

int stdd(int list[], int n) {
  int sum = 0;
  for (int i = 0; i < n; ++i) {
    sum += list[i];
  }
  int mean = sum / n;
  int ret = 0;
  for (int i = 0; i < n; ++i) {
    int diff = list[i] - mean;
    ret += diff * diff;
  }
  return mysqrt(ret / n);
}

void printstats(int list[], int n) {
  if (n == 0) {
    printf(2, "Empty List!\n");
    exit();
  }
  int sum = 0;
  for (int i = 0; i < n; ++i) {
    sum += list[i];
  }
  selectionSort(list, n);
  printf(1, "Standard Deviation = %f\n", stdd(list, n));
  printf(1, "Average = %f\n", sum / n);
  printf(1, "Median = %f\n", list[n / 2]);
  printf(1, "Min = %f\n", list[0]);
  printf(1, "Max = %f\n", list[n - 1]);
}

int main(int argc, char *argv[]) {
  argc = argc - 1;
  int *list = malloc(sizeof(int) * argc);
  for (int i = 0; i < argc; ++i) {
    char *arg = argv[i + 1];
    list[i] = 100 * atoi(arg);
    advance(&arg, '.');
    int mul = 1;
    if (*arg) {
      if (!arg[1]) {
        mul = 10;
      } else {
        arg[2] = 0;
      }
    }
    list[i] += mul * atoi(arg);
  }
  printstats(list, argc);
  exit();
}
