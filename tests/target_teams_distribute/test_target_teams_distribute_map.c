//===--- test_target_teams_distribute_map.c----------------------------------===//
//
// OpenMP API Version 4.5 Nov 2015
//
// This test uses the map clause on a target teams distribute clause to test that
// the indicated data is maped on the target in the fashion that is indicated
// with the map-type.  Each map-type that is valid in this situation (to, from,
// alloc, and tofrom) are each used and tested.
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
  int errors;

  for (int x = 0; x < 1024; ++x) {
      a[x] = x;
      b[x] = 0;
  }

  #pragma omp target enter data map(alloc: b[0:1024])
  #pragma omp target teams distribute map(to: a[0:1024], b[0:1024])
  for (int x = 0; x < 1024; ++x){
      b[x] = a[x];
  }
  #pragma omp target exit data map(from: b[0:1024])

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != b[x]);
      if (a[x] != b[x]){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      b[x] = 0;
  }

  #pragma omp target enter data map(to: a[0:1024])
  #pragma omp target teams distribute map(from: b[0:1024])
  for (int x = 0; x < 1024; ++x){
      b[x] = a[x];
  }
  #pragma omp target exit data map(delete: a[0:1024])

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

  #pragma omp target enter data map(to: a[0:1024]) map(alloc: b[0:1024])
  #pragma omp target teams distribute map(alloc: a[0:1024], b[0:1024], c[0:1024])
  for (int x = 0; x < 1024; ++x){
      c[x] = a[x];
      b[x] = c[x];
  }
  #pragma omp target exit data map(delete: a[0:1024]) map(from: b[0:1024])

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, a[x] != b[x]);
      if (a[x] != b[x]){
          break;
      }
  }

  for (int x = 0; x < 1024; ++x){
      b[x] = x;
  }

  #pragma omp target enter data map(to: a[0:1024])
  #pragma omp target teams distribute map(tofrom: b[0:1024])
  for (int x = 0; x < 1024; ++x){
      b[x] += a[x];
  }
  #pragma omp target exit data map(delete: a[0:1024])

  for (int x = 0; x < 1024; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, b[x] != 2 * x);
      if (b[x] != 2 * x){
          break;
      }
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
