// RUN: %libomptarget-compile-run-and-check-aarch64-unknown-linux-gnu
// RUN: %libomptarget-compile-run-and-check-powerpc64-ibm-linux-gnu
// RUN: %libomptarget-compile-run-and-check-powerpc64le-ibm-linux-gnu
// RUN: %libomptarget-compile-run-and-check-x86_64-pc-linux-gnu

#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int test_defaultmap_off() {
  
  int errors = 0;
  
  // we try with all the scalars
  enum { VAL1 = 1, VAL2, VAL3, VAL4} scalar_enum = VAL1;
  
  int isHost = 0; 
  // Map the same array to multiple devices. initialize with device number
  #pragma omp target map(from: isHost)
    {
      isHost = omp_is_initial_device();
      scalar_enum = VAL4;
    } // end of omp target 
  
  // If it is initial device then we will modify the original memory region
  if (isHost) {
    errors += scalar_enum != VAL4;
  } else {
    errors += scalar_enum != VAL1;
  }
  
  return errors;
}


int main() {
  int errors = 0;
  errors += test_defaultmap_off();

  printf("errors = %d \n", errors);

  return 0;
}

