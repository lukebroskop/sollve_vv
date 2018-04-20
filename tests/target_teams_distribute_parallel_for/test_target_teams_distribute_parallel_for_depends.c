// RUN: %libomptarget-compile-run-and-check-aarch64-unknown-linux-gnu
// RUN: %libomptarget-compile-run-and-check-powerpc64-ibm-linux-gnu
// RUN: %libomptarget-compile-run-and-check-powerpc64le-ibm-linux-gnu
// RUN: %libomptarget-compile-run-and-check-x86_64-pc-linux-gnu

#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define N 1000

#define DEPEND_OUT_TARGET_BIT 0x1
#define DEPEND_INOUT_HOST_BIT 0x2
#define DEPEND_INOUT_TARGET_BIT 0x4
#define DEPEND_IN_TARGET_BIT 0x8
#define DEPEND_ALL_BITS 0xF

int test_all_dependencies() {
  OMPVV_INFOMSG("test_all_dependencies");

  int errors = 0;
  int dep_1[N], dep_2[N];

  // Initialize dep_1 and dep_2
  for (int i = 0; i < N; ++i) {
    dep_1[i] = 0;
    dep_2[i] = 0;
  }

  // testing depend(out... )
#pragma omp target teams distribute parallel for \
  depend(out: dep_1) map(tofrom: dep_1[0:N])
  for (int i = 0; i < N; i++) {
    dep_1[i] |= DEPEND_OUT_TARGET_BIT;
  }

#pragma omp target teams distribute parallel for \
  depend(out: dep_2) map(tofrom: dep_2[0:N])
    for (int i = 0; i < N; i++) {
      dep_2[i] |= DEPEND_OUT_TARGET_BIT;
    }

  // testing depend(inout... ) with host
#pragma omp task depend(inout: dep_1) \
  depend(inout: dep_2) shared(dep_1, dep_2)
  {
    for (int i = 0; i < N; ++i) {
      dep_1[i] |= DEPEND_INOUT_HOST_BIT;
      dep_2[i] |= DEPEND_INOUT_HOST_BIT;
    }
  }

  // Testing denpend(inout... ) with device 
#pragma omp target teams distribute parallel for \
  depend(inout: dep_1, dep_2) map(tofrom: dep_1[0:N], dep_2[0:N])
  for (int i = 0; i < N; i++) {
    dep_1[i] |= DEPEND_INOUT_TARGET_BIT;
    dep_2[i] |= DEPEND_INOUT_TARGET_BIT;
  }

  // Testing denpend(in... )
#pragma omp target teams distribute parallel for \
  depend(in: dep_1) map(tofrom: dep_1[0:N])
  for (int i = 0; i < N; i++) {
    dep_1[i] |= DEPEND_IN_TARGET_BIT;
  }
  
#pragma omp target teams distribute parallel for \
  depend(in: dep_2) map(tofrom: dep_2[0:N])  
  for (int i = 0; i < N; i++) {
    dep_2[i] |= DEPEND_IN_TARGET_BIT;
  }

#pragma omp taskwait
 
  // Checking the results 
  for (int i = 0; i < N; i++) {
    OMPVV_TEST_AND_SET(errors, dep_1[i] != DEPEND_ALL_BITS);
    OMPVV_TEST_AND_SET(errors, dep_2[i] != DEPEND_ALL_BITS);    
  }

  return errors;
}

int main() {
  OMPVV_TEST_OFFLOADING;
  int errors = 0;
  OMPVV_TEST_AND_SET_VERBOSE(errors, test_all_dependencies());

  OMPVV_REPORT_AND_RETURN(errors);
}

