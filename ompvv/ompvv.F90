!===------ ompvv.F90 ------------------ OMPVV HEADER FILE FORTRAN --------===//
! 
! Header file for OMP Validation and verification test suite
! 
!===----------------------------------------------------------------------===//
#define __LINENUMBER__ __LINE__
!#define __FILENAME__ __FILE__(INDEX(__FILE__,"/", back=.true.)+1:)
#define __FILENAME__ __FILE__
#define OMPVV_HEADER_FMT(header, message) '("[header ",A,":",I0,"] ",message)'
#define OMPVV_HEADER_RESULT_FMT '("[OMPVV_RESULT ",A,"] Test ",A," on the ",A,".")'

! Macro for output of information, warning and error messages
#ifdef VERBOSE_MODE
#define OMPVV_WARNING_HELPER(message, filename, line) WRITE(*, OMPVV_HEADER_FMT(OMPVV_WARNING,message)) filename, line
#define OMPVV_WARNING(message) OMPVV_WARNING_HELPER(message,__FILENAME__,__LINE__)
#define OMPVV_WARNING_IF(condition, message) IF (condition) OMPVV_WARNING(message)
#define OMPVV_ERROR_HELPER(message, filename, line) WRITE(ERROR_UNIT, OMPVV_HEADER_FMT(tMPVV_ERROR,message)) filename, line
#define OMPVV_ERROR(message) OMPVV_ERROR_HELPER(message,__FILENAME__,__LINE__)
#define OMPVV_ERROR_IF(condition, message) IF (condition) OMPVV_ERROR(message)
#define OMPVV_INFOMSG_HELPER(message, filename, line) WRITE(*, OMPVV_HEADER_FMT(OMPVV_INFOMSG,message)) filename, line
#define OMPVV_INFOMSG(message) OMPVV_INFOMSG_HELPER(message, __FILENAME__, __LINE__)
#define OMPVV_INFOMSG_IF(condition, message) IF (condition) OMPVV_INFOMSG(message)
! END IF VERBOSE_MODE
#endif

! NONE VERBOSE MODE
#ifndef VERBOSE_MODE

#define OMPVV_WARNING_HELPER(message, filename, line) CONTINUE ! message, filename, line
#define OMPVV_WARNING(message) CONTINUE !message
#define OMPVV_WARNING_IF(condition, message) CONTINUE !message
#define OMPVV_ERROR_HELPER(message, filename, line) CONTINUE ! message, filename, line
#define OMPVV_ERROR(message) CONTINUE !message
#define OMPVV_ERROR_IF(condition, message) CONTINUE !message
#define OMPVV_INFOMSG_HELPER(message, filename, line) CONTINUE ! message, filename, line
#define OMPVV_INFOMSG(message) CONTINUE !message
#define OMPVV_INFOMSG_IF(condition, message) CONTINUE !message

! END IF VERBOSE_MODE
#endif

! Macro for checking if offloading is enabled or not
#define OMPVV_TEST_OFFLOADING test_offloading(__FILENAME__, __LINE__)

! Macro for checking if offloading is enabled or not and set a variable with the result
#define OMPVV_TEST_AND_SET_OFFLOADING(var2set) var2Set = test_and_set_offloading(__FILENAME__, __LINE__)

! Macro for testing for errors
#define OMPVV_TEST(condition) test_error(condition, __FILENAME__, __LINE)

! Macro for setting errors on condition
#define OMPVV_TEST_AND_SET(err, condition) err = test_and_set(condition, __FILENAME__, __LINE__)

! Macro for setting errors on condition and displaying an error if something went wrong
#define OMPVV_TEST_AND_SET_VERBOSE(err, condition) err = test_and_set_verbose(condition, "condition", __FILENAME__, __LINE__)

! Macro for reporting results
#define OMPVV_REPORT() report_errors(__FILE_NAME__)

! Macro for correct exit code
#define OMPVV_RETURN(err) EXIT MERGE(EXIT_SUCCESS, EXIT_FAILURE, errors == 0)

#define OMPVV_REPORT_AND_RETURN(err) EXIT MERGE(EXIT_SUCCESS, EXIT_FAILURE,  report_and_set_errors(__FILENAME__))

! Macro to report warning if it is a shared environment 
#define OMPVV_TEST_SHARED_ENVIRONMENT test_shared_environment(__FILENAME__, __LINE__)

! Macro to report warning if it is a shared environment and set a variable for further use
#define OMPVV_TEST_AND_SET_SHARED_ENVIRONMENT(var2set) var2set = test_and_set_shared_environment(__FILENAME__, __LINE__)

! Auxiliar module used for standarized output and testing
module ompvv_lib
  use omp_lib
  implicit none
    LOGICAL :: isHost
    INTEGER :: errors = 0
    LOGICAL :: sharedEnv
  contains 
    ! Sets the isHost variable checking if it is device or hosts
    subroutine test_offloading_probe()
      isHost = .false.
!$omp target map(from:isHost)
      isHost = omp_is_initial_device()
!$omp end target 
    end subroutine test_offloading_probe

    ! test offloading prints if offloading is enabled or not
    subroutine test_offloading(fn ,ln)
      CHARACTER(len=*) :: fn 
      INTEGER :: ln

      ! Avoid unused variables warning 
      fn = fn
      ln = ln

      call test_offloading_probe()
      IF (isHost) THEN
        OMPVV_INFOMSG_HELPER("Target region executed on the host",fn , ln)
      ELSE
        OMPVV_INFOMSG_HELPER("Target region executed on the device",fn , ln)
      END IF
    end subroutine test_offloading  

    ! This function check if offloading is enabled and return true or false
    function test_and_set_offloading(fn , ln)
      CHARACTER(len=*) :: fn 
      INTEGER :: ln
      LOGICAL :: test_and_set_offloading

      call test_offloading(fn, ln)
      test_and_set_offloading = .not. isHost
    end function test_and_set_offloading

    ! Function to test an error and register in the error variable
    subroutine test_error(condition, fn, ln)
      LOGICAL, INTENT(IN) :: condition
      CHARACTER(len=*) :: fn
      INTEGER :: ln

      ! Avoid unused variables warning
      fn = fn 
      ln = ln
      
      IF (condition) errors = errors + 1
    end subroutine test_error
    
    ! Function to test an error condition and report it to the user
    subroutine test_error_verbose(condition, conditionStr, fn, ln)
      LOGICAL, INTENT(IN) :: condition
      CHARACTER(len=*) :: conditionStr
      CHARACTER(len=*) :: fn
      INTEGER :: ln

      ! Avoid unused variables warning
      fn = fn 
      ln = ln
      conditionStr = conditionStr
      
      IF (condition) then 
        errors = errors + 1
        OMPVV_ERROR_HELPER(" Condition "//conditionStr//" failed ", fn, ln)
      END IF 
    end subroutine test_error_verbose

    ! Function to test an error condition and return the current value of errors
    function test_and_set(condition, fn, ln)
      LOGICAL, INTENT(IN) :: condition
      CHARACTER(len=*), INTENT(IN) :: fn
      INTEGER, INTENT(IN) :: ln
      INTEGER :: test_and_set

      call test_error(condition, fn, ln)

      test_and_set = errors
    end function test_and_set

    ! Function to test an error condition, return the current value of errors
    ! and report the current value to the user STDIO
    function test_and_set_verbose(condition, conditionStr, fn, ln) 
      LOGICAL, INTENT(IN) :: condition
      CHARACTER(len=*) :: conditionStr
      CHARACTER(len=*) :: fn
      INTEGER :: ln
      INTEGER :: test_and_set_verbose 

      call test_error_verbose(condition, conditionStr, fn, ln)

      test_and_set_verbose = errors
    end function test_and_set_verbose

    ! Function to report errors
    subroutine report_errors(fn)
      CHARACTER(len=*) :: fn
      CHARACTER(len=50) :: message2dis

      WRITE(message2dis, *) "The value of errors is ", errors
      message2dis = TRIM(message2dis)

      IF (errors /= 0) then 
        OMPVV_INFOMSG(message2dis)
        IF (isHost) THEN 
          WRITE(*,OMPVV_HEADER_RESULT_FMT) fn, "failed", "host"
        ELSE
          WRITE(*,OMPVV_HEADER_RESULT_FMT) fn, "failed", "device"
        END IF
      ELSE
        OMPVV_INFOMSG("Test ran with no errorS")
        IF (isHost) THEN 
          WRITE(*,OMPVV_HEADER_RESULT_FMT) fn, "passed", "host"
        ELSE
          WRITE(*,OMPVV_HEADER_RESULT_FMT) fn, "passed", "device"
        END IF
      END IF

    end subroutine report_errors

    function report_and_set_errors(fn)
      CHARACTER(len=*) :: fn
      INTEGER :: report_and_set_errors

      call report_errors(fn)

      report_and_set_errors = errors
    end function report_and_set_errors

    ! Macro to check if it is a shared data environment
    subroutine test_shared_environment_probe()
      INTEGER :: isSharedProb

      isSharedProb = 0
      sharedEnv = .false.

      ! checking for isHost
      call test_offloading_probe()

!omp target map(to: isSharedProb)
      isSharedProb = 1
!omp end target
      
      IF (.not. isHost .and. isSharedProb == 1)  sharedEnv = .true.
    end subroutine test_shared_environment_probe

    subroutine test_shared_environment(fn, ln)
      CHARACTER(len=*) :: fn
      INTEGER :: ln

      ! Avoid unused variables warning
      fn = fn
      ln = ln

      call test_shared_environment_probe()
      if (sharedEnv) OMPVV_WARNING_HELPER("This tests is running on a shared data environment between host and device. This may cause errors", fn, ln)
    end subroutine test_shared_environment

    function test_and_set_shared_environment(fn, ln)
      CHARACTER(len=*), INTENT(IN) :: fn
      INTEGER, INTENT(IN) :: ln
      LOGICAL :: test_and_set_shared_environment

      call test_shared_environment(fn, ln)

      test_and_set_shared_environment = sharedEnv

    end function test_and_set_shared_environment


end module ompvv_lib
