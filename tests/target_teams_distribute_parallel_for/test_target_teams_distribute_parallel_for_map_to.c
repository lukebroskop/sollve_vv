//===---- test_target_teams_distribute_parallel_for_map_default.c - combined consutrct -===//
// 
// OpenMP API Version 4.5 Nov 2015
// 
// testing the maping to of arrays and a non-variable. Making sure that it does not copy it 
// over 
//
//===-----------------------------------------------------------------------------------===//

#include <omp.h>
#include "ompvv.h"
#include <stdio.h>

#define SIZE_N 2000

int test_target_teams_distribute_parallel_for_map_to() {
  OMPVV_INFOMSG("test_target_teams_distribute_parallel_for_map_to");
  
  int a[SIZE_N];
  int b[SIZE_N];
  int d[SIZE_N];
  int scalar = 50; // This one is to test the to of an scalar
  int errors = 0;
  int i, j, dev;

  // array initialization
  for (i = 0; i < SIZE_N; i++) {
    a[i] = 1;
    b[i] = i;
    d[i] = 0;
  }

  // check multiple sizes. 
#pragma omp target teams distribute parallel for map(to: a, b, scalar) map(tofrom: d)
    for (j = 0; j < SIZE_N; ++j) {
      d[j] = (a[j] + b[j]) * scalar;
    }

  for (i = 0; i < SIZE_N; i++) {
    OMPVV_TEST_AND_SET(errors, d[i] != (1 + i) * 50);
  }

  return errors;
}

// Test for OpenMP 4.5 target enter data with if
int main() {
  int isSharedEnv;
  OMPVV_TEST_OFFLOADING;
  OMPVV_TEST_AND_SET_SHARED_ENVIRONMENT(isSharedEnv);
  int errors = 0;

  OMPVV_WARNING_IF(isSharedEnv, "Shared memory environment makes this test inconclusive");

  OMPVV_TEST_AND_SET_VERBOSE(errors, test_target_teams_distribute_parallel_for_map_to());

  OMPVV_REPORT_AND_RETURN(errors);
}
