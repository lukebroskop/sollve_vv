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
  int privatized = 0;
  int privatized_array[2];
  int errors = 0;

  for (int x = 0; x < 1024; ++x){
      a[x] = 1;
      b[x] = x;
      c[x] = 0;
  }

  #pragma omp target data map(to: a[0:1024], b[0:1024]) map(tofrom: c[0:1024])
  {
      #pragma omp target teams distribute lastprivate(privatized)
      for (int x = 0; x < 1024; ++x){
          privatized = a[x] - b[x];
          c[x] = privatized + b[x];
      }
  }

  for (int x = 0; x < 1024; ++x){
      if (c[x] - a[x] != 0){
          errors += 1;
      }
  }
  if (privatized != a[1023] - b[1023]){
      errors += 1;
  }

  for (int x = 0; x < 1024; ++x){
      a[x] = 1;
      b[x] = x;
      c[x] = x % 10;
  }

  #pragma omp target data map(to: a[0:1024], b[0:1024], c[0:1024])
  {
      #pragma omp target teams distribute lastprivate(privatized_array)
      for (int x = 0; x < 1024; ++x){
          privatized_array[0] = a[x] + b[x] + c[x];
          privatized_array[1] = (a[x] + b[x]) * c[x];
      }
  }

  if (privatized_array[0] != (a[1023] + b[1023] + c[1023])){
      errors += 1;
  }
  if (privatized_array[1] != ((a[1023] + b[1023]) * c[1023])){
      errors += 1;s
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
