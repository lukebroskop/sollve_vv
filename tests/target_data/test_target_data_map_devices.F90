!===---- test_target_data_map_devices.F90 - using target data with multiple dev --===//
! 
! OpenMP API Version 4.5 Nov 2015
! 
! Iterating over the number of devices, this tests uses target data with
! multiple devices using the devices clause or using the set default device
! clause
!
!===-------------------------------------------------------------------------===//
#include "ompvv.F90"

#define N 5000

      PROGRAM test_target_data_devices
        USE iso_fortran_env
        USE ompvv_lib
        USE omp_lib
        implicit none
        LOGICAL :: isOffloading, isSharedEnv
        INTEGER :: i,j
        CHARACTER(len=500) :: msgHelper

        OMPVV_TEST_AND_SET_OFFLOADING(isOffloading)
        OMPVV_TEST_AND_SET_SHARED_ENVIRONMENT(isSharedEnv)

        OMPVV_TEST_VERBOSE(test_set_default_device() .NE. 0)
        OMPVV_TEST_VERBOSE(test_device_clause() .NE. 0)

        OMPVV_REPORT_AND_RETURN()

        CONTAINS 
          ! Testing set default device API
          INTEGER FUNCTION test_set_default_device()
            INTEGER :: num_dev, def_dev, errors, devData devComp
            INTEGER :: errors_bf, errors_af
            INTEGER dimensions(N) :: anArray

            OMPVV_INFOMSG("using set_default_device")

            num_dev = omp_get_num_devices()
            def_dev = omp_get_default_device()
            WRITE(msgHelper, '(A,I0)') "number of devices = ", num_dev
            OMPVV_INFOMSG(msgHelper)
            WRITE(msgHelper, '(A,I0)') "default device = ", def_dev
            OMPVV_INFOMSG(msgHelper)
            
            ! Initialize the array
            anArray(:) = 1

            ! Iterate over all the devices
            DO devData = 0, num_dev - 1
              ! set default device
              omp_set_defalt_device(devData)
              !$omp target data map(tofrom: anArray(1:N))

                ! Iterate over all the devices doing comp
                ! to guarantee that the right data env is 
                ! being copied back and forth
                DO devComp = 0, num_dev - 1
                  !$omp target map(alloc: anArray(1:N)) &
                  !$omp device(devComp)

                    ! Increase only in the current tested device
                    IF (devComp == devData) THEN
                      anArray(1:N) = anArray(1:N) + 1
                    ELSE IF (.NOT. isSharedEnv)
                      ! This should not affect result as it should happen in
                      ! other devices
                      anArray(1:N) = anArray(1:N) - 10
                    END IF
                    
                  !$omp end target
                END DO devComp
              !$end omp target data
            END DO devData
              
            OMPVV_GET_ERRORS(errors_bf)
            OMPVV_TEST_VERBOSE(ANY(anArray /= num_dev + 1))
            OMPVV_GET_ERRORS(errors_af)

            ! return the default device to the 
            ! original default 
            omp_set_defalt_device(def_dev)

            test_set_default_device = errors_bf - errors_af

          END FUNCTION test_set_default_device
          ! Testing set device with deviceclause
          INTEGER FUNCTION test_device_clause()
          INTEGER :: num_dev, def_dev, errors, devData devComp
          INTEGER :: errors_bf, errors_af
          INTEGER dimensions(N) :: anArray

          OMPVV_INFOMSG("using device() clause")

            num_dev = omp_get_num_devices()
            WRITE(msgHelper, '(A,I0)') "number of devices = ", num_dev
            OMPVV_INFOMSG(msgHelper)
            
            ! Initialize the array
            anArray(:) = 1

            ! Iterate over all the devices
            DO devData = 0, num_dev - 1
              !$omp target data map(tofrom: anArray(1:N)) &
              !$omp device(devData)

              ! Iterate over all the devices doing comp
              ! to guarantee that the right data env is 
              ! being copied back and forth
                DO devComp = 0, num_dev - 1
                  !$omp target map(alloc: anArray(1:N)) &
                  !$omp device(devComp)
                    ! Increase only in the current tested device
                    IF (devComp == devData) THEN
                      anArray(1:N) = anArray(1:N) + 1
                    ELSE IF (.NOT. isSharedEnv)
                      ! This should not affect result as it should happen in
                      ! other devices
                      anArray(1:N) = anArray(1:N) - 10
                    END IF
                  !$omp end target
                END DO devComp
              !$end omp target data
            END DO devData
              
            OMPVV_GET_ERRORS(errors_bf)
            OMPVV_TEST_VERBOSE(ANY(anArray /= num_dev + 1))
            OMPVV_GET_ERRORS(errors_af)

            test_device_clause = errors_bf - errors_af

          END FUNCTION test_device_clause
      END PROGRAM

  
