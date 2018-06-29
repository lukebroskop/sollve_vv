PROGRAM offloading_success
!INCLUDE ompvv.f90
!USE ompvv
USE omp_lib
implicit none
  INTEGER :: errors
  LOGICAL :: isHost

  isHost = .false.

!$omp target map(from:isHost)
  isHost = omp_is_initial_device()
!$omp end target 

  ! CHECK: Target region executed on the device
  IF (isHost) THEN
    errors = 1
    print*, "Target region executed on the host"
  ELSE
    errors = 0
    print*, "Target region executed on the device"
  END IF

  CALL EXIT(errors)

END PROGRAM offloading_success
