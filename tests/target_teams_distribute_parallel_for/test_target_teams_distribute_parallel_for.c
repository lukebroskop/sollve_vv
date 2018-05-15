#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define ARRAY_SIZE 1024

int test_target_teams_distribute_parallel_for() {
  OMPVV_INFOMSG("test_target_teams_distribute_parallel_for");

  // Variable for errors counting
  int errors = 0;

  int a[ARRAY_SIZE];
  int b[ARRAY_SIZE];
  int num_teams = 0;
  int num_threads[ARRAY_SIZE];
  int alert_num_threads = 0;
  int is_host;
  int x;

  // a and b array initialization
  for (x = 0; x < ARRAY_SIZE; ++x) {
      a[x] = 1;
      b[x] = x;
      num_threads[x] = 0;
  }

  #pragma omp target data map(tofrom: a[0:ARRAY_SIZE], num_teams, is_host) map(to: b[0:ARRAY_SIZE])
  {
      #pragma omp target teams distribute
      for (x = 0; x < ARRAY_SIZE; ++x){
          is_host = omp_is_initial_device();
          num_teams = omp_get_num_teams();
          num_threads[x] = omp_get_num_threads();
          a[x] += b[x];
      }
  }


  for (x = 0; x < ARRAY_SIZE; ++x){
      OMPVV_TEST_AND_SET(errors, (a[x] != 1 + b[x]));
      if (num_threads[x] == 1) {
        alert_num_threads++;
      }
  }

  // Rise lack of parallelism alerts
  if (num_teams == 1) {
      OMPVV_WARNING("Test operated with one team.  Parallelism of teams distribute can't be guarunteed.");
  }
  if (alert_num_threads == ARRAY_SIZE) {
      OMPVV_WARNING("Test operated with one thread in all the teams. Parallelism of threads within a team can't be guaranteed");
  }

  return errors;
}

// Test for OpenMP 4.5 target data with if
int main() {
  OMPVV_TEST_OFFLOADING;

  int errors = 0;

  OMPVV_TEST_AND_SET_VERBOSE(errors, test_target_teams_distribute_parallel_for());

  OMPVV_REPORT_AND_RETURN(errors);
}
