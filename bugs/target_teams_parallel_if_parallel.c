#include <omp.h>
#include <stdio.h>

#define SIZE_N 10

int main() {
  
  int num_threads[SIZE_N], execution_place[SIZE_N];
  int i;
  
  // Initialize a and warning
  for (i = 0; i < SIZE_N; i++) {
    num_threads[i] = 1;
    execution_place[i] = 0;
  }
  int condition = 0;
#pragma omp target teams distribute parallel for \
    map(tofrom: num_threads, execution_place) num_threads(10) if(parallel: condition)
    for (i = 0; i < SIZE_N; i++) {
      num_threads[i] = omp_get_num_threads();
      execution_place[i] = omp_is_initial_device();
    }

    printf(" = num_threads \t execution_place = \n");
  for (i = 0; i < SIZE_N; i++) {
    printf("     %d \t   %s \n", num_threads[i], (execution_place[i]? "host": "device"));;
  }

  return 0;
}

