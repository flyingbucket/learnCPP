#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

int compare_int(const void* a, const void* b) {
  int arg1 = *(const int*)a;
  int arg2 = *(const int*)b;

  if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}

int main() {
  int arr[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
  size_t len = sizeof(arr) / sizeof(arr[0]);

  HeapSort((char*)arr, sizeof(int), len, compare_int);

  for (size_t i = 0; i < len; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");

  return 0;
}
