//===--- test_target_teams_distribute.c--------------------------------------===//
//
// OpenMP API Version 4.5 Nov 2015
//
// This test uses the target teams distribute directive and tests to validate
// that computation inside the region executes properly.
//
////===----------------------------------------------------------------------===//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define ARRAY_SIZE 1024

int main() {
  int a[ARRAY_SIZE];
  int b[ARRAY_SIZE];
  int num_teams[ARRAY_SIZE];
  int errors = 0;
  int is_offloading;

  OMPVV_TEST_AND_SET_OFFLOADING(is_offloading);
  // a and b array initialization
  for (int x = 0; x < ARRAY_SIZE; ++x) {
    a[x] = 1;
    b[x] = x;
    num_teams[x] = -1;
  }

#pragma omp target data map(tofrom: a[0:ARRAY_SIZE], num_teams[0:ARRAY_SIZE]) map(to: b[0:ARRAY_SIZE])
  {
#pragma omp target teams distribute map(alloc: a[0:ARRAY_SIZE], b[0:ARRAY_SIZE], num_teams[0:ARRAY_SIZE])
    for (int x = 0; x < ARRAY_SIZE; ++x) {
      num_teams[x] = omp_get_num_teams();
      a[x] += b[x];
    }
  }


  for (int x = 0; x < ARRAY_SIZE; ++x) {
    if (x > 0 && num_teams[x] != num_teams[x - 1]) {
      OMPVV_ERROR("Test reported an inconsistent number of teams between loop iterations.");
      errors += 1;
    }
    OMPVV_TEST_AND_SET_VERBOSE(errors, (a[x] != 1 + b[x]));
    if (a[x] != 1 + b[x]){
      break;
    }
  }

  if (num_teams[0] == 1) {
    OMPVV_WARNING("Test operated with one team.  Parallelism of teams distribute can't be guaranteed.");
  }

  if (!is_offloading) {
    OMPVV_WARNING("Test operated on host.  Target region was ignored.");
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
