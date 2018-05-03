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
  int default_threads = 0;
  int num_threads = 0;
  int errors = 0;

  // a and b array initialization
  for (int x = 0; x < 1024; ++x) {
      a[x] = 1;
      b[x] = x;
      c[x] = 0;
  }

  #pragma omp target data map(tofrom: default_threads)
  {
      #pragma omp target teams distribute
      for (int x = 0; x < 1024; ++x){
          default_threads = omp_get_thread_limit();
      }
  }

  if (default_threads == 1){
      OMPVV_WARNING("Test operated with one thread.  Testing of thread_limit clause cannot be done.");
  }
  else if(default_threads == 0){
      OMPVV_ERROR("Test returned thread_limit == 0.  Maybe omp_get_thread_limit() is not returning correct number of teams.");
      errors = 1;
  }
  else{
      #pragma omp target data map(to: a[0:1024], b[0:1024]) map(from: c[0:1024], num_threads)
      {
          #pragma omp target teams distribute thread_limit(default_threads - 1)
          for (int x = 0; x < 1024; ++x){
              num_threads = omp_get_thread_limit();
          }
      }
      if (num_threads > default_threads - 1){
          errors += 1;
          OMPVV_ERROR("Test ran on more threads than requested.");
          return errors;
      }
      else if (num_threads < default_threads - 1){
          OMPVV_WARNING("Test ran on less threads than requested.  Still spec-conformant.");
      }
      else{
          OMPVV_INFOMSG("Test passed with offloading %s", (isOffloading ? "enabled" : "disabled"));
      }
  }
  OMPVV_REPORT_AND_RETURN(errors);
}
