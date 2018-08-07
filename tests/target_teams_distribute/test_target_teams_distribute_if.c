//===--- test_target_teams_distribute_if.c-----------------------------------===//
//
// OpenMP API Version 4.5 Nov 2015
//
// This test uses the if clause to specify whether the target teams distribute
// directve should execute.  If it does not and the test operates in an
// environment with a device that has separate memory from the host, then
// the test uses the separate memory to validate where the region inside the
// target teams distribute directive executed and also tests to make sure that
// operation of the data directives not on the target teams distribute directive
// still function.
//
// If not operating on a device with separate memory, the test has a minimal test
// of the basic use of the if clause with both a true and a false parameter.
// However, the execution is identical to that of host operation in both cases.
//
////===----------------------------------------------------------------------===//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define SIZE_THRESHOLD 512

int main() {
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  int a[1024];
  int b[1024];
  int devtest = 1;
  int errors = 0;


  #pragma omp target enter data map(to: devtest)
  #pragma omp target
  {
      devtest = 0;
  }

  for (int x = 0; x < 1024; ++x) {
      a[x] = 1;
      b[x] = x;
  }

  if (devtest == 1){
      //There is a separate memory device, full data environment tests can procede
      #pragma omp target data map(tofrom: a[0:1024]) map(to: b[0:1024])
      {
          #pragma omp target teams distribute if(b[0] > 1) map(alloc: devtest, a[0:1024], b[0:1024])
          for (int x = 0; x < 1024; ++x){
              a[x] += b[x] + devtest;
          }
      }

      for (int x = 0; x < 1024; ++x){
          OMPVV_TEST_AND_SET_VERBOSE(errors, (a[x] != 1));
          if (a[x] != 1){
              printf("%d at %d\n", a[x], x);
              break;
          }
      }

      #pragma omp target data map(tofrom: a[0:1024]) map(to: b[0:1024])
      {
          #pragma omp target teams distribute if(b[0] < 1) map(alloc: devtest, a[0:1024], b[0:1024])
          for (int x = 0; x < 1024; ++x){
              a[x] += b[x] + devtest;
          }
      }

      for (int x = 0; x < 1024; ++x){
          OMPVV_TEST_AND_SET_VERBOSE(errors, (a[x] != 1 + b[x]));
          if (a[x] != 1 + b[x]){
              printf("%d at %d\n", a[x], x);
              break;
          }
      }

      #pragma omp target data map(to: a[0:1024], b[0:1024])
      {
          #pragma omp target teams distribute if(b[0] > 1) map(alloc: devtest, a[0:1024], b[0:1024])
          for (int x = 0; x < 1024; ++x){
              a[x] += b[x] + devtest;
          }
      }

      for (int x = 0; x < 1024; ++x){
          OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != 2 + 2 * b[x]);
          if (a[x] != 2 + 2 * b[x]){
              printf("%d at %d\n", a[x], x);
              break;
          }
      }
  }
  else{
      #pragma omp target data map(tofrom: a[0:1024]) map(to: b[0:1024])
      {
          #pragma omp target teams distribute if(b[0] > 1) map(alloc: devtest, a[0:1024], b[0:1024])
          for (int x = 0; x < 1024; ++x){
              a[x] += b[x] + devtest;
          }
      }

      for (int x = 0; x < 1024; ++x){
          OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != b[x] + 1);
          if (a[x] != b[x] + 1){
              break;
          }
      }

      #pragma omp target data map(tofrom: a[0:1024]) map(to: b[0:1024])
      {
          #pragma omp target teams distribute if(b[0] < 1) map(alloc: devtest, a[0:1024], b[0:1024])
          for (int x = 0; x < 1024; ++x){
              a[x] += b[x] + devtest;
          }
      }

      for (int x = 0; x < 1024; ++x){
          OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != 2 * b[x]);
          if (a[x] != 2 * b[x]){
              break;
          }
      }
  }

  #pragma omp target exit data map(delete: devtest)

  OMPVV_REPORT_AND_RETURN(errors);
}
