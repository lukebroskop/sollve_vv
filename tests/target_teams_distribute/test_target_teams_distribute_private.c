//===--- test_target_teams_distribute_private.c------------------------------===//
//
// OpenMP API Version 4.5 Nov 2015
//
// This test uses the private clause on a target teams distribute directive to
// indicate that the variable in the private clause should be made private to
// each team executing the teams distribute region.  The test then operates on
// the privatized variable in such a way that would most likely cause competing
// operations if the variable is not privatized.  If the computation completes
// without errors, we assume that the privatization occured.
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
  int privatized;
  int ishost;
  int errors = 0;

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
          privatized = 0;
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

  OMPVV_REPORT_AND_RETURN(errors);
}
