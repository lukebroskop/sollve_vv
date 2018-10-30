!===--- test_target_teams_distribute_if.F90---------------------------------===//
!
! OpenMP API Version 4.5 Nov 2015
!
! This test uses the if clause to specify whether the target teams distribute
! directve should execute.  If it does not and the test operates in an
! environment with a device that has separate memory from the host, then
! the test uses the separate memory to validate where the region inside the
! target teams distribute directive executed and also tests to make sure that
! operation of the data directives not on the target teams distribute directive
! still function.
!
! If not operating on a device with separate memory, the test has a minimal test
! of the basic use of the if clause with both a true and a false parameter.
! However, the execution is identical to that of host operation in both cases.
!
!//===----------------------------------------------------------------------===//

#include "ompvv.F90"

#define N 1024

      PROGRAM test_target_teams_distribute_if
        USE iso_fortran_env
        USE ompvv_lib
        USE omp_lib
        implicit none
        INTEGER :: errors
        LOGICAL,DIMENSION(1):: devtest
        OMPVV_TEST_OFFLOADING()
        errors = 0

        devtest(1) = .TRUE.
        !$omp target enter data map(to: devtest(1:1))
        !$omp target map(alloc: devtest(1:1))
          devtest(1) = .FALSE.
        !$omp end target

        IF (devtest(1)) THEN
          OMPVV_TEST_VERBOSE(test_separate_mem_pt_a() .ne. 0)
          OMPVV_TEST_VERBOSE(test_separate_mem_pt_b() .ne. 0)
          OMPVV_TEST_VERBOSE(test_separate_mem_pt_c() .ne. 0)
        ELSE
          OMPVV_TEST_VERBOSE(test_shared_mem_pt_a() .ne. 0)
          OMPVV_TEST_VERBOSE(test_shared_mem_pt_b() .ne. 0)
        END IF

        OMPVV_REPORT_AND_RETURN()
      CONTAINS
        INTEGER FUNCTION test_separate_mem_pt_a()
          INTEGER,DIMENSION(N):: a, b
          INTEGER:: x, errors

		      errors = 0

          DO x = 1, N
            a(x) = 1
            b(x) = x
          END DO

          !$omp target data map(tofrom: a(1:N)) map(to: b(1:N))
            !$omp target teams distribute if(b(1) .gt. 2) map(alloc: a(1:N), &
            !$omp& b(1:N))
            DO x = 1, N
              a(x) = a(x) + b(x)
            END DO
          !$omp end target data

          DO x = 1, N
            IF (a(x) .ne. 1) THEN
              errors = errors + 1
            END IF
          END DO

          test_separate_mem_pt_a = errors
        END FUNCTION test_separate_mem_pt_a

        INTEGER FUNCTION test_separate_mem_pt_b()
          INTEGER,DIMENSION(N):: a, b
          INTEGER :: x, errors

		      errors = 0

          DO x = 1, N
            a(x) = 1
            b(x) = x
          END DO

          !$omp target data map(tofrom: a(1:N)) map(to: b(1:N))
            !$omp target teams distribute if(b(1) .lt. 2) map(alloc: a(1:N), &
            !$omp& b(1:N))
            DO x = 1, N
              a(x) = a(x) + b(x)
            END DO
          !$omp end target data

          DO x = 1, N
            IF (a(x) .ne. 1 + x) THEN
              errors = errors + 1
            END IF
          END DO

          test_separate_mem_pt_b = errors
        END FUNCTION test_separate_mem_pt_b

        INTEGER FUNCTION test_separate_mem_pt_c()
          INTEGER,DIMENSION(N) :: a, b
          INTEGER :: x, errors
          LOGICAL :: is_host

		      errors = 0

          DO x = 1, N
            a(x) = 1
            b(x) = x
          END DO

          !$omp target data map(to: a(1:N), b(1:N))
            !$omp target teams distribute if(b(1) .gt. 2) map(alloc: a(1:N), &
            !$omp& b(1:N))
            DO x = 1, N
              a(x) = a(x) + b(x)
              is_host = omp_is_initial_device()
            END DO
          !$omp end target data

          DO x = 1, N
            IF (a(x) .ne. 1 + x) THEN
              errors = errors + 1
              OMPVV_ERROR("ERROR")
            END IF
          END DO

          IF (is_host .neqv. .TRUE.) THEN
            errors = errors + 1
            OMPVV_ERROR("ERROR")
          END IF

          test_separate_mem_pt_c = errors
        END FUNCTION test_separate_mem_pt_c

        INTEGER FUNCTION test_shared_mem_pt_a()
          INTEGER:: errors, x
          LOGICAL:: is_host
          INTEGER,DIMENSION(N):: a, b

		      errors = 0

          DO x = 1, N
            a(x) = 1
            b(x) = x
          END DO

          !$omp target data map(tofrom: a(1:N), is_host) map(to: b(1:N))
            !$omp target teams distribute if(b(1) .gt. 2) map(alloc: a(1:N), &
            !$omp& b(1:N))
            DO x = 1, N
              a(x) = a(x) + b(x)
              is_host = omp_is_initial_device()
            END DO
          !$omp end target data

          DO x = 1, N
            IF (a(x) .ne. x + 1) THEN
              errors = errors + 1
            END IF
          END DO

          IF (is_host .neqv. .TRUE.) THEN
            errors = errors + 1
          END IF

          test_shared_mem_pt_a = errors
        END FUNCTION test_shared_mem_pt_a

        INTEGER FUNCTION test_shared_mem_pt_b()
          INTEGER:: errors, x
          INTEGER,DIMENSION(N):: a, b

		      errors = 0

          DO x = 1, N
            a(x) = 1
            b(x) = x
          END DO

          !$omp target data map(tofrom: a(1:N)) map(to: b(1:N))
            !$omp target teams distribute if(b(1) .lt. 2) map(alloc: a(1:N), &
            !$omp& b(1:N))
            DO x = 1, N
              a(x) = a(x) + b(x)
            END DO
          !$omp end target data

          DO x = 1, N
            IF (a(x) .ne. x + 1) THEN
              errors = errors + 1
            END IF
          END DO


          test_shared_mem_pt_b = errors
        END FUNCTION test_shared_mem_pt_b
      END PROGRAM test_target_teams_distribute_if
