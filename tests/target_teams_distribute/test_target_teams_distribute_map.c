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
  int errors;

  // a and b array initialization
  for (int x = 0; x < 1024; ++x) {
      a[x] = x;
      b[x] = 0;
  }

  #pragma omp target data map(from: b[0:1024])
  {
      #pragma omp target teams distribute map(to: a[0:1024])
      for (int x = 0; x < 1024; ++x){
          b[x] = a[x];
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != b[x]);
      if (a[x] != b[x]){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      b[x] = 0;
  }

  #pragma omp target data map(to: a[0:1024])
  {
      #pragma omp target teams distribute map(from: b[0:1024])
      for (int x = 0; x < 1024; ++x){
          b[x] = a[x];
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != b[x]);
      if (a[x] != b[x]){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      b[x] = 0;
      c[x] = 0;
  }

  #pragma omp target data map(to: a[0:1024]) map(from: b[0:1024])
  {
      #pragma omp target teams distribute map(alloc: c[0:1024])
      for (int x = 0; x < 1024; ++x){
          c[x] = a[x];
          b[x] = c[x];
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != b[x]);
      if (a[x] != b[x]){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      b[x] = x;
  }

  #pragma omp target data map(to: a[0:1024])
  {
      #pragma omp target teams distribute map(tofrom: b[0:1024])
      for (int x = 0; x < 1024; ++x){
          b[x] += a[x];
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, b[x] != 2 * x);
      if (b[x] != 2 * x){
          break;
      }
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
