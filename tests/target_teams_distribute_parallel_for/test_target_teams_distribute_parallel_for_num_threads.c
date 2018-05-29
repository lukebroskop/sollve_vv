//===---- tests_target_teams_distribute_parallel_for_num_threads.c     ------===//
// 
// OpenMP API Version 4.5 Nov 2015
// 
// Test to check the num_threads clause. This clause changes the upper limit of 
// the number of threads inside the parallel region. 
//
//===----------------------------------------------------------------------===//

#include <omp.h>
#include "ompvv.h"
#include <stdio.h>

#define SIZE_N 1024

int test_target_teams_distribute_parallel_for_num_threads() {
  OMPVV_INFOMSG("test_target_teams_distribute_parallel_for_num_threads");
 
  int tested_num_threads[] = {1, 10, 100, 10000};
  int num_threads[SIZE_N]; // num_threads = 1 is not technically an error
  int errors = 0;
  int i, nt;


// Trying multiple num_threads values from 1 to a large number
for (nt = 0; nt < 4; nt++) {

  // Initializing the num_threads array
  for (i = 0; i < SIZE_N; i++) {
    num_threads[i] = -1;
  }
#pragma omp target teams distribute parallel for\
        map(tofrom: num_threads) num_threads(tested_num_threads[nt])
    for (i = 0; i < SIZE_N; i++) {
      num_threads[i] = omp_get_num_threads();
    }

  int raiseWarning = 0;
  
  for (i = 0; i < SIZE_N; i++) {
    // If the number of threads is larger than the specified, this is an error
    OMPVV_TEST_AND_SET(errors, num_threads[i] > tested_num_threads[nt]);
    if (tested_num_threads[nt] != 1 && num_threads[i] == 1) {
      raiseWarning = 1;
    }
  }
  OMPVV_WARNING_IF(raiseWarning != 0, "The number of threads was 1 even though num_threads(%d) was used. This is not a compliance error in the specs", tested_num_threads[nt]);  
}

  return errors;
}

// Test for OpenMP 4.5 target enter data with if
int main() {
  OMPVV_TEST_OFFLOADING;
  int errors = 0;

  OMPVV_TEST_AND_SET_VERBOSE(errors, test_target_teams_distribute_parallel_for_num_threads());

  OMPVV_REPORT_AND_RETURN(errors);
}
