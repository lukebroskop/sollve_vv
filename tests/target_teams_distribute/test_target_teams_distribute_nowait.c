//===--- test_target_teams_distribute_nowait.c-------------------------------===//
//
// OpenMP API Version 4.5 Nov 2015
//
// This test uses the nowait clause on a target teams distribute directive and
// uses a barrier to resyncronize the target regions.  Since we can't be sure
// that operations will be asyncronous, we can not test to make sure that
// the regions are executed asynchronously.
//
////===----------------------------------------------------------------------===//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define SIZE_THRESHOLD 512
#define ITERATIONS 1024
int main() {
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  int a[1024];
  int b[1024];
  int c[1024];
  int d[1024];
  int e[1024];
  int f[1024];
  int errors = 0;
  int is_host;
  int race_condition_found = 0;

  for (int y = 0; y < ITERATIONS; ++y){
      for (int x = 0; x < 1024; ++x) {
          a[x] = x + y;
          b[x] = 2 * x + y;
          c[x] = 0;
          d[x] = 3 * x + y;
          e[x] = 4 * x + y;
          f[x] = 0;
          g[x] = 0;
      }

      #pragma omp target data map(to: a[0:1024], b[0:1024], d[0:1024], e[0:1024]) map(from: c[0:1024], f[0:1024]) map(tofrom: is_host)
      {
          #pragma omp target teams distribute nowait map(alloc: a[0:1024], b[0:1024], c[0:1024])
          for (int x = 0; x < 1024; ++x){
              c[x] = a[x] + b[x];
          }
          #pragma omp target teams distribute nowait map(alloc: is_host, c[0:1024], d[0:1024], e[0:1024], f[0:1024])
          for (int x = 0; x < 1024; ++x){
              is_host = omp_is_initial_device();
              f[x] = c[x] + d[x] + e[x];
          }
      }

      for (int x = 0; x < 1024; ++x){
          OMPVV_TEST_AND_SET_VERBOSE(errors, c[x] != 3 * x + 2 * y);
          if (f[x] != 10 * x + 4 * y){
              race_condition_found = 1
          }
      }
  }

  OMPVV_WARNING_IF(race_conidition_found == 0, "Could not show that nowait was operating on target teams distribute construct");

  OMPVV_REPORT_AND_RETURN(errors);
}
