//===--- test_target_teams_distribute_depend_array_section.c-----------------===//
//
// OpenMP API Version 4.5 Nov 2015
//
// This test checks if out-out dependency works even if the data in the list
// is an array section. Both tasks are given the nowait clause to allow
// for the possibility that they will be incorrectly run out of order. If the
// two target teams ditribute loops run out of order, the test fails.
//
////===----------------------------------------------------------------------===//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define N 1024

int test_target_teams_distribute_depend_array_section() {
  int isOffloading = 0;
  int a[N];
  int b[N];
  int c[N];
  int d[N];

  for (int x = 0; x < N; ++x) {
    a[x] = x;
    b[x] = 2 * x;
    c[x] = 0;
    d[x] = 0;
  }

#pragma omp target data map(to: a[0:N], b[0:N]) map(alloc: c[0:N]) map(from: d[0:N])
  {
#pragma omp target teams distribute nowait depend(out: c[0:N - 1]) map(alloc: a[0:N], b[0:N], c[0:N])
    for (int x = 0; x < N; ++x) {
      c[x] = a[x] + b[x];
    }
#pragma omp target teams distribute nowait depend(out: c[0:N - 1]) map(alloc: b[0:N], c[0:N], d[0:N])
    for (int x = 0; x < N; ++x) {
      d[x] = c[x] + b[x];
    }
  }

  for (int x = 0; x < N - 1; ++x) {
    if (d[x] != 5 * x) {
      OMPVV_ERROR("Test of depend clause using array sections did not pass with offloading %s", (isOffloading ? "enabled" : "disabled"));
      return 1;
    }
  }
  return 0;
}

int main() {
  int errors = 0;
  int isOffloading = 0;
  OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
  errors += test_target_teams_distribute_depend_array_section();
  if (errors != 0) {
    OMPVV_INFOMSG("Test passed with offloading %s", (isOffloading ? "enabled" : "disabled"));
  }
  OMPVV_REPORT_AND_RETURN(errors);
}
