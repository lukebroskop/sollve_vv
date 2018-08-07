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

int main() {
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  int a[1024];
  int b[1024];
  int c[1024];
  int d[1024];
  int e[1024];
  int f[1024];
  int g[1024];
  int errors = 0;
  int is_host;

  for (int x = 0; x < 1024; ++x) {
      a[x] = x;
      b[x] = 2 * x;
      c[x] = 0;
      d[x] = 3 * x;
      e[x] = 4 * x;
      f[x] = 0;
      g[x] = 0;
  }

  #pragma omp target data map(to: a[0:1024], b[0:1024], d[0:1024], e[0:1024]) map(from: c[0:1024], f[0:1024], g[0:1024]) map(tofrom: is_host)
  {
      #pragma omp parallel
      {
          #pragma omp target teams distribute nowait map(alloc: a[0:1024], b[0:1024], c[0:1024])
          for (int x = 0; x < 1024; ++x){
              c[x] = a[x] + b[x];
          }
          #pragma omp target teams distribute nowait map(alloc: d[0:1024], e[0:1024], f[0:1024])
          for (int x = 0; x < 1024; ++x){
              f[x] = d[x] + e[x];
          }
          #pragma omp barrier
          #pragma omp target teams distribute map(alloc: is_host, c[0:1024], f[0:1024], g[0:1024])
          for (int x = 0; x < 1024; ++x){
              is_host = omp_is_initial_device();
              g[x] = c[x] + f[x];
          }
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, c[x] != 3 * x);
      if (c[x] != 3 * x){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, f[x] != 7 * x);
      if (f[x] != 7 * x){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, g[x] != 10 * x);
      if (g[x] != 10 * x){
          break;
      }
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
