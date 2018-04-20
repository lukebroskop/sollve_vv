//===---- test_target_teams_distribute_parallel_for_map_tofrom.c - combined consutrct -===//
// 
// OpenMP API Version 4.5 Nov 2015
// 
//
//===-----------------------------------------------------------------------------------===//

#include <omp.h>
#include "ompvv.h"
#include <stdio.h>

#define SIZE_N 2000
#define ITERATIONS 1000

int test_target_teams_distribute_parallel_for_map_tofrom() {
  OMPVV_INFOMSG("test_target_teams_distribute_parallel_for_map_tofrom");
  
  int a[SIZE_N];
  int b[SIZE_N];
  int c[SIZE_N];
  int d[SIZE_N];
  int scalar = 50;
  int errors = 0;
  int i, j, dev;


  // check multiple times
  for (i = 0; i < ITERATIONS; ++i) {
    // variables initialization
    scalar = 50;
    for (i = 0; i < SIZE_N; i++) {
      a[i] = 1;
      b[i] = i;
      c[i] = 2*i;
      d[i] = 0;
    }

    // Tests
#pragma omp target teams distribute parallel for map(tofrom: a, b, c, d, scalar)
    for (j = 0; j < SIZE_N; ++j) {
      d[j] += c[j] * (a[j] + b[j] + scalar);
      a[j] = 10;
      b[j] = 11;
      c[j] = 12;
      scalar = 13;
    }

    // Checking the results
    OMPVV_TEST_AND_SET(errors, scalar != 13);
    for (i = 0; i < SIZE_N; i++) {
      OMPVV_TEST_AND_SET(errors, a[i] != 10);
      OMPVV_TEST_AND_SET(errors, b[i] != 11);
      OMPVV_TEST_AND_SET(errors, c[i] != 12);
      OMPVV_TEST_AND_SET(errors, d[i] != (1 + i + 50)*2*i);
    }
  }

  return errors;
}

// Test for OpenMP 4.5 target enter data with if
int main() {
  OMPVV_TEST_OFFLOADING;
  int errors = 0;

  OMPVV_TEST_AND_SET_VERBOSE(errors, test_target_teams_distribute_parallel_for_map_to());

  OMPVV_REPORT_AND_RETURN(errors);
}
