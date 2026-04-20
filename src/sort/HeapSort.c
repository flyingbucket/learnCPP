#include <stddef.h>

#include "sort.h"
void swap(char* vec, size_t elem_size, size_t x, size_t y) {
  if (x == y) return;
  char* x_ptr = vec + x * elem_size;
  char* y_ptr = vec + y * elem_size;
  for (size_t i = 0; i < elem_size; i++) {
    char tmp = x_ptr[i];
    x_ptr[i] = y_ptr[i];
    y_ptr[i] = tmp;
  }
}

void HeapAdjust(char* vec, size_t elem_size, size_t i, size_t len,
                int (*cmp)(const void*, const void*)) {
  size_t largest = i;
  size_t left = 2 * i + 1;
  size_t right = left + 1;

  if (left < len &&
      cmp(vec + left * elem_size, vec + largest * elem_size) > 0) {
    largest = left;
  }
  if (right < len &&
      cmp(vec + right * elem_size, vec + largest * elem_size) > 0) {
    largest = right;
  }

  if (largest != i) {
    swap(vec, elem_size, largest, i);
    HeapAdjust(vec, elem_size, largest, len, cmp);
  }
}

void BuildMaxHeap(char* vec, size_t elem_size, size_t len,
                  int (*cmp)(const void*, const void*)) {
  for (size_t i = len / 2; i > 0; i--) {
    HeapAdjust(vec, elem_size, i - 1, len, cmp);
  }
}

void HeapSort(char* vec, size_t elem_size, size_t len,
              int (*cmp)(const void*, const void*)) {
  BuildMaxHeap(vec, elem_size, len, cmp);
  for (size_t i = len - 1; i > 0; i--) {
    swap(vec, elem_size, i, 0);
    HeapAdjust(vec, elem_size, 0, i, cmp);
  }
}
