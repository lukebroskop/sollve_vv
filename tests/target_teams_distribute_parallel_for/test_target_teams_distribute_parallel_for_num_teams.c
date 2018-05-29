//===---- tests_target_teams_distribute_parallel_for_num_teams.c     ------===//
// 
// OpenMP API Version 4.5 Nov 2015
// 
// Test to check the num_teams clause. This clause changes the upper limit of 
// the number of teams inside the target teams region. 
//
//===----------------------------------------------------------------------===//

#include <omp.h>
#include "ompvv.h"
#include <stdio.h>

#define SIZE_N 1024

int test_target_teams_distribute_parallel_for_num_teams() {
  OMPVV_INFOMSG("test_target_teams_distribute_parallel_for_num_teams");
 
  int tested_num_teams[] = {1, 10, 100, 10000};
  int num_teams[SIZE_N]; // num_teams = 1 is not technically an error
  int errors = 0;
  int i, nt;


// Trying multiple num_teams values from 1 to a large number
for (nt = 0; nt < 4; nt++) {

  // Initializing the num_teams array
  for (i = 0; i < SIZE_N; i++) {
    num_teams[i] = -1;
  }
#pragma omp target teams distribute parallel for\
        map(tofrom: num_teams) num_teams(tested_num_teams[nt])
    for (i = 0; i < SIZE_N; i++) {
      num_teams[i] = omp_get_num_teams();
    }

  int raiseWarning = 0;
  
  for (i = 0; i < SIZE_N; i++) {
    // If the number of teams is larger than the specified, this is an error
    OMPVV_TEST_AND_SET(errors, num_teams[i] > tested_num_teams[nt]);
    if (tested_num_teams[nt] != 1 && num_teams[i] == 1) {
      raiseWarning = 1;
    }
  }
  OMPVV_WARNING_IF(raiseWarning != 0, "The number of teams was 1 even though num_teams(%d) was used. This is not a compliance error in the specs", tested_num_teams[nt]);  
}

  return errors;
}

// Test for OpenMP 4.5 target enter data with if
int main() {
  OMPVV_TEST_OFFLOADING;
  int errors = 0;

  OMPVV_TEST_AND_SET_VERBOSE(errors, test_target_teams_distribute_parallel_for_num_teams());

  OMPVV_REPORT_AND_RETURN(errors);
}
