#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define SIZE_THRESHOLD 512

// Test for OpenMP 4.5 target data with if
int main() {
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  int a[1024];
  int b[1024];
  int c[1024];
  int d[1024];
  int privatized;
  int ishost;
  int errors = 0;

  // a and b array initialization
  for (int x = 0; x < 1024; ++x) {
      a[x] = 1;
      b[x] = x;
      c[x] = 2*x;
      d[x] = 0;
  }

  #pragma omp target data map(from: d[0:1024]) map(to: a[0:1024], b[0:1024], c[0:1024])
  {
      #pragma omp target teams distribute private(privatized) map(alloc: a[0:1024], b[0:1024], c[0:1024], d[0:1024])
      for (int x = 0; x < 1024; ++x){
          for (int y = 0; y < a[x] + b[x]; ++y){
              privatized++;
          }
          d[x] = c[x] * privatized;
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, d[x] != (1 + x) * 2 * x);
      if (d[x] != (1 + x)*2*x){
          break;
      }
  }

  OMPVV_REPORT_AND_RETURN((errors[0] + errors[1]));
}
