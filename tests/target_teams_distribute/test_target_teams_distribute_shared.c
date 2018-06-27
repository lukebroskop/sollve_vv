#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define SIZE 1024

// Test for OpenMP 4.5 target data with if
int main() {
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  int a[SIZE];
  int share = 0;
  int errors = 0;
  int num_teams;

  // a array initialization
  for (int x = 0; x < SIZE; ++x) {
      a[x] = x;
  }

  #pragma omp target data map(to: a[0:SIZE]) map(tofrom: share, num_teams)
  {
      #pragma omp target teams distribute shared(share)
      for (int x = 0; x < SIZE; ++x){
          num_teams = omp_get_num_teams();
          #pragma omp atomic
          share = share + a[x];
      }
  }

  for (int x = 0; x < SIZE; ++x){
      share = share - x;
  }

  OMPVV_TEST_AND_SET_VERBOSE(errors, (share != 0));
  OMPVV_ERROR_IF(errors, "The value of share is = %d", share);

  share = -1;
  #pragma omp target data map(to: a[0:SIZE]) map(tofrom: share)
  {
      #pragma omp target teams distribute shared(share)
      for (int x = 0; x < SIZE; ++x){
          share = a[x];
      }
  }

  OMPVV_TEST_AND_SET_VERBOSE(errors, (share < 0 || share >= SIZE));
  OMPVV_ERROR_IF(errors, "The value of share is = %d", share); 

  share = 5;

  #pragma omp target data map(tofrom: a[0:SIZE]) map(tofrom: share)
  {
      #pragma omp target teams distribute shared(share)
      for (int x = 0; x < SIZE; ++x){
          a[x] = a[x] + share;
      }
  }

  for (int x = 0; x < SIZE; ++x){
      OMPVV_TEST_AND_SET_VERBOSE(errors, (a[x] - 5 != x));
  }

  if (num_teams == 1){
      OMPVV_WARNING("Test operated on one team, results of default shared test are inconclusive.");
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
