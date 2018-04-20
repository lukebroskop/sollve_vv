// RUN: %libomptarget-compile-run-and-check-aarch64-unknown-linux-gnu
// RUN: %libomptarget-compile-run-and-check-powerpc64-ibm-linux-gnu
// RUN: %libomptarget-compile-run-and-check-powerpc64le-ibm-linux-gnu
// RUN: %libomptarget-compile-run-and-check-x86_64-pc-linux-gnu

#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int test_defaultmap_on() {

  int errors = 0;

  // we try with all the scalars
  char scalar_char = 'a';
  short scalar_short = 10;
  int scalar_int = 11;
  float scalar_float = 5.5f;
  double scalar_double = 10.45;
  enum { VAL1 = 1, VAL2, VAL3, VAL4} scalar_enum = VAL1;
  

  // Map the same array to multiple devices. initialize with device number
#pragma omp target defaultmap(tofrom: scalar)
  {
    scalar_char = 'b';
    scalar_short = 20;
    scalar_int = 33;
    scalar_float = 6.5f;
    scalar_double = 20.45;
    scalar_enum = VAL4;
  } // end of omp target 

  errors += scalar_char != 'b';
  errors += scalar_short != 20;
  errors += scalar_int != 33;
  errors += scalar_float != 6.5f;
  errors += scalar_double != 20.45;
  errors += scalar_enum != VAL4;
  
  return errors;
}

int test_defaultmap_off() {
  
  int errors = 0;
  
  // we try with all the scalars
  char scalar_char = 'a';
  short scalar_short = 10;
  int scalar_int = 11;
  float scalar_float = 5.5f;
  double scalar_double = 10.45;
  enum { VAL1 = 1, VAL2, VAL3, VAL4} scalar_enum = VAL1;
  
  int isHost = 0; 
  // Map the same array to multiple devices. initialize with device number
  #pragma omp target map(from: isHost)
    {
      isHost = omp_is_initial_device();
      scalar_char = 'b';
      scalar_short = 20;
      scalar_int = 33;
      scalar_float = 6.5f;
      scalar_double = 20.45;
      scalar_enum = VAL4;
    } // end of omp target 
  
  // If it is initial device then we will modify the original memory region
  if (isHost) {
    errors += scalar_char != 'b';
    errors += scalar_short != 20;
    errors += scalar_int != 33;
    errors += scalar_float != 6.5f;
    errors += scalar_double != 20.45;
    errors += scalar_enum != VAL4;
  } else {
    errors += scalar_char != 'a';
    errors += scalar_short != 10;
    errors += scalar_int != 11;
    errors += scalar_float != 5.5f;
    errors += scalar_double != 10.45;
    errors += scalar_enum != VAL1;
  }
  
  return errors;
}


int main() {
  int errors = 0;
  errors += test_defaultmap_on();
  errors += test_defaultmap_off();

  printf("errors = %d \n", errors);

  return 0;
}

