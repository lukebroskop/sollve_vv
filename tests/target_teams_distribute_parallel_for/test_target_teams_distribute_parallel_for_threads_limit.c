//===---- tests_target_teams_distribute_parallel_for_thread_limit.c     ------===//
// 
// OpenMP API Version 4.5 Nov 2015
// 
// Test to check the thread_limit clause. This clause changes the upper limit of 
// the number of threads inside each of the contention groups created in the teams
// region. This upper limit is different to the num_threads 
//
//===----------------------------------------------------------------------===//

#include <omp.h>
#include "ompvv.h"
#include <stdio.h>

#define SIZE_N 1024

int test_target_teams_distribute_parallel_for_threads_limit() {
  OMPVV_INFOMSG("test_target_teams_distribute_parallel_for_threads_limit");

  int tested_num_threads[] = {1, 10, 100, 10000};
  int tested_thread_limit[] = {1, 10, 100, 10000};
  int num_threads[SIZE_N];
  int thread_limit[SIZE_N];
  int errors = 0;
  int i, nt, tl;


// Trying multiple num_threads and thread_limits
// values from 1 to a large number. The number of threads
// should never be larger than the thread limit
for (nt = 0; nt < 4; nt++) {
  for (tl = 0; tl < 4; tl++) {

    // Initializing the num_threads and thread_limit array
    for (i = 0; i < SIZE_N; i++) {
      num_threads[i] = -1;
      thread_limit[i] = -1;
    }

#pragma omp target teams distribute parallel for map(tofrom: num_threads) \
        num_threads(tested_num_threads[nt]) thread_limit(tested_thread_limit[tl])
      for (i = 0; i < SIZE_N; i++) {
        num_threads[i] = omp_get_num_threads();
        thread_limit[i] = omp_get_thread_limit();
      }
    
    for (i = 0; i < SIZE_N; i++) {
      // If the thread limit is not the one specified or the actual number 
      // of threads is greater than the thread limit, then it is an error
      OMPVV_TEST_AND_SET(errors, thread_limit[i] != tested_thread_limit[nt]);
      OMPVV_TEST_AND_SET(errors, num_threads[i] > tested_thread_limit[nt]);
    }
  }
}

  return errors;
}

// Test for OpenMP 4.5 target enter data with if
int main() {
  OMPVV_TEST_OFFLOADING;
  int errors = 0;

  OMPVV_TEST_AND_SET_VERBOSE(errors, test_target_teams_distribute_parallel_for_thread_limit());

  OMPVV_REPORT_AND_RETURN(errors);
}
