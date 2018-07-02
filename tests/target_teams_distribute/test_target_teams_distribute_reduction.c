#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define SIZE_THRESHOLD 512


int test_add_general(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    int a[1024];
    int b[1024];
    int total = 0;
    int host_total = 0;
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        a[x] = 1;
        b[x] = x;
    }

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024], b[0:1024])
    {
        #pragma omp target teams distribute reduction(+:total)
        for (int x = 0; x < 1024; ++x){
            num_teams = omp_get_num_teams();
            total += a[x] + b[x];
        }
    }

    for (int x = 0; x < 1024; ++x){
        host_total += a[x] + b[x];
    }

    if (host_total != total){
        errors = errors = 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_and(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    char a[1024];
    char result;
    char host_result;
    double false_margin = pow(exp(1), log(.5)/1024);
    int errors = 0;
    int num_teams = 0;

    for (int itr_count = 0; itr_count < 1024; ++itr_count){
        for (int x = 0; x < 1024; ++x){
            if (rand() / (double)(RAND_MAX) < false_margin){
                a[x] = 1;
            }
            else{
                a[x] = 0;
            }
        }

        result = 1;
        host_result = 1;

        #pragma omp target data map(to: a[0:1024]) map(tofrom: num_teams)
        {
            #pragma omp target teams distribute reduction(&&:result)
            for (int x = 0; x < 1024; ++x){
                num_teams = omp_get_num_teams();
                result = result && a[x];
            }
        }

        for (int x = 0; x < 1024; ++x){
            host_result = host_result && a[x];
        }

        if (host_result != result){
            errors += 1;
        }
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_bitand(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    unsigned int a[1024];
    double false_margin = pow(exp(1), log(.5)/1024);
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        for (int y = 0; y < 16; ++y){
            if (rand() / (real_t) RAND_MAX < false_margin){
                a[x] += 1 << y
            }
        }
    }

    unsigned int b = 0
    for (int x = 0; x < 16; ++x){
        b = b + (1 + << x);
    }

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024])
    {
        #pragma omp target teams distribute reduction(&:b)
        for (int x = 0; x < 1024; ++x){
            num_teams = omp_get_num_teams();
            b = b & a[x];
        }
    }
    unsigned int host_b = a[0];

    for (int x = 0; x < 1024; ++x){
        host_b = host_b & a[x];
    }

    if (b != host_b){
        errors = errors + 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_bitor(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    int a[1024];
    double false_margin = pow(exp(1), log(.5)/1024);
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        for (int y = 0; y < 16; ++y){
            if (rand() / (double) RAND_MAX > false_margin){
                a[x] += (1 << y);
            }
        }
    }

    unsigned int b = 0;

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024])
    {
        #pragma omp target teams distribute reduction(|:b)
        for (int x = 0; x < 1024; ++x){
            num_teams = omp_get_num_teams();
            b = b | a[x];
        }
    }

    unsigned int host_b = 0;

    for (int x = 0; x < 1024; ++x){
        host_b = host_b | a[x];
    }

    if (b != host_b){
        errors = 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_bitxor(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    unsigned int a[1024];
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        a[x] = (unsigned int) rand() / (double) (RAND_MAX / 2);
    }

    unsigned int b = 0;

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024])
    {
        #pragma omp target teams distribute reduction(^:b)
        for (int x = 0; x < 1024; ++x){
            num_teams = omp_get_num_teams();
            b = (b ^ a[x]);
        }
    }

    unsigned int host_b = 0;

    for (int x = 0; x < 1024; ++x){
        host_b = (host_b ^ a[x]);
    }

    if (host_b != b){
        errors = 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_max(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    int a[1024];
    int b[1024];
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        a[x] = (int) rand() / (double)(RAND_MAX / 100);
        b[x] = (int) rand() / (double)(RAND_MAX / 100);
    }

    int result = 0;

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024], b[0:1024])
    {
        #pragma omp target teams distribute reduction(max:result)
        for (int x = 0; x < 1024; ++x){
            result = max(a[x] + b[x], result);
            num_teams = omp_get_num_teams();
        }
    }

    int host_max = 0;

    for (int x = 0; x < 1024; ++x){
        host_max = max(host_max, a[x] + b[x]);
    }

    if (result != host_max){
        errors += 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_min(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    int a[1024];
    int b[1024];
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        a[x] = (int) rand() / (double) (RAND_MAX / 100);
        b[x] = (int) rand() / (double) (RAND_MAX / 100);
    }

    int result = a[0] + b[0];

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024], b[0:1024])
    {
        #pragma omp target teams distribute reduction(min:result)
        for (int x = 0; x < 1024; ++x){
            num_teams = omp_get_num_teams();
            result = min(result, a[x] + b[x]);
        }
    }

    int host_min = a[0] + b[0];

    for (int x = 0; x < 1024; ++x){
        host_min = min(host_min, a[x] + b[x]);
    }

    if (host_min != result){
        errors = errors + 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }
    return errors;
}

int test_multiply(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    int a[1024];
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        a[x] = 1 + (int) rand() / (real_t) RAND_MAX;
    }

    int result = 1;

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024])
    {
        for (int x = 0; x < 1024; x = x + 16){
            result = 1;
            #pragma omp target teams distribute reduction(*:result)
            for (int y = 0; y < 16; ++y){
                result *= a[x];
            }
            host_result = 1;
            for (int y = 0; y < 16; ++y){
                host_result *= a[x];
            }
            if (host_result != result){
                errors = errors + 1;
            }
        }
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }

    return errors;
}

int test_or(){
    int isOffloading = 0;
    OMPVV_TEST_AND_SET_OFFLOADING(isOffloading);
    char a[1024];
    double false_margin = pow(exp(1), log(.5)/1024);
    int errors = 0;
    int num_teams = 0;

    for (int x = 0; x < 1024; ++x){
        if (rand() / (double)(RAND_MAX) > false_margin){
            a[x] = 1;
        }
        else{
            a[x] = 0;
        }
    }

    char result = 0;

    #pragma omp target data map(tofrom: num_teams) map(to: a[0:1024])
    {
        #pragma omp target teams distribute reduction(||:result)
        for (int x = 0; x < 1024; ++x){
            num_teams = omp_get_num_teams();
            result = result || a[x];
        }
    }

    char host_result = 0;
    for (int x = 0; x < 1024; ++x){
        host_result = host_result || a[x];
    }

    if (host_result != result){
        errors += 1;
    }

    if (num_teams == 1){
        OMPVV_WARNING("Test operated with one team.  Testing of reduction clause cannot be done.");
    }

    return errors;
}





// Test for OpenMP 4.5 target data with if
int main() {
  int total_errors = 0;
  total_errors += test_add();
  total_errors += test_and();
  total_errors += test_bitand();
  total_errors += test_bitor();
  total_errors += test_bitxor();
  total_errors += test_max();
  total_errors += test_min();
  total_errors += test_multiply();
  total_errors += test_or();
  OMPVV_REPORT_AND_RETURN(errors);
}