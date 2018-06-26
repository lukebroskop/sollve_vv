#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define SIZE_THRESHOLD 512

// Test for OpenMP 4.5 target data with if
int main() {
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  int a[1024];
  int share = 0;
  int errors = 0;
  int num_teams;

  // a and b array initialization
  for (int x = 0; x < 1024; ++x) {
      a[x] = x;
  }

  #pragma omp target data map(to: a[0:1024]) map(tofrom: share, num_teams)
  {
      #pragma omp target teams distribute shared(share)
      for (int x = 0; x < 1024; ++x){
          num_teams = omp_get_num_teams();
          #pragma omp atomic
          share = share + a[x];
      }
  }

  for (int x = 0; x < 1024; ++x){
      share = share - x;
  }
  if (share != 0){
      errors = errors + 1;
  }
  share = -1;
  #pragma omp target data map(to: a[0:1024]) map(tofrom: share)
  {
      #pragma omp target teams distribute shared(share)
      for (int x = 0; x < 1024; ++x){
          share = a[x];
      }
  }

  if (share < 0 || share > 1023){
      errors = errors + 1;
  }

  share = 5;

  #pragma omp target data map(tofrom: a[0:1024]) map(tofrom: share)
  {
      #pragma omp target teams distribute shared(share)
      for (int x = 0; x < 1024; ++x){
          a[x] = a[x] + share;
      }
  }

  for (int x = 0; x < 1024; ++x){
      if (a[x] - 5 != x){
          errors = errors + 1;
      }
  }

  if (num_teams == 1){
      OMPVV_WARNING("Test operated on one team, results of default shared test are inconclusive.");
  }

  OMPVV_REPORT_AND_RETURN(errors);
}
