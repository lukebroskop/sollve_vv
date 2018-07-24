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
  int privatized_array[10];
  int privatized = 0;
  int ishost;
  int errors = 0;

  // a and b array initialization
  for (int x = 0; x < 1024; ++x) {
      a[x] = 1;
      b[x] = x;
      c[x] = 2*x;
      d[x] = 0;
  }

  for (int x = 0; x < 10; ++x){
      privatized_array[x] = x;
  }

  //Test privitization of data in firstprivate clause
  #pragma omp target data map(from: d[0:1024]) map(to: a[0:1024], b[0:1024], c[0:1024])
  {
      #pragma omp target teams distribute firstprivate(privatized) map(alloc: a[0:1024], b[0:1024], c[0:1024], d[0:1024])
      for (int x = 0; x < 1024; ++x){
          for (int y = 0; y < a[x] + b[x]; ++y){
              privatized++;
          }
          d[x] = c[x] * privatized;
          privatized = 0;
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, d[x] != (1 + x) * 2 * x);
      if (d[x] != (1 + x) * 2 * x){
          break;
      }
  }
  //Test initialization of data in firstprivate clause
  #pragma omp target data map(from: d[0:1024]) map(to: a[0:1024], b[0:1024], c[0:1024])
  {
      #pragma omp target teams distribute firstprivate(privatized_array) map(alloc: a[0:1024], b[0:1024], c[0:1024], d[0:1024])
      for (int x = 0; x < 1024; ++x){
          d[x] = a[x] + b[x] + c[x] + privatized_array[x%10];
      }
  }

  int temp;
  for (int x = 0; x < 1024; ++x){
      temp = x % 10;
      OMPVV_TEST_AND_SET_VERBOSE(errors, d[x] != 1 + 3 * x + temp);
      if (d[x] != 1 + 3 * x + (x%10)){
          break;
      }
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
