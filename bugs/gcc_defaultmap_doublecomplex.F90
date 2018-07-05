      PROGRAM default_map_rep
        USE iso_fortran_env
        implicit none
        COMPLEX(kind = 16) :: scalar_double_complex
        CHARACTER :: scalar_char
        LOGICAL :: firstprivateCheck (2)


        ! initialization
        scalar_char = 'a'
#ifdef DOUBLE_COMPLEX
        scalar_double_complex = (1e+10, 1e+10)
#endif
        !$omp target map(from: firstprivateCheck)
          ! Checking the 'firstprivate behavior'
          firstprivateCheck(1) = scalar_char == 'a'
          scalar_char = 'b'
#ifdef DOUBLE_COMPLEX
          firstprivateCheck(2) = scalar_double_complex == (1e+10, 1e+10)
          scalar_double_complex = (5e+9, 5e+9)
#endif
        !$omp end target 

        ! checking that the original value was not changed
        IF (.not. firstprivateCheck(1)) PRINT*, "scalar_char not&
        &firstprivatized"
        IF (scalar_char /= 'a') PRINT*, "The value of scalar_char &
        &changed and it should not have"
#ifdef DOUBLE_COMPLEX
        IF (.not. firstprivateCheck(2)) PRINT*, "scalar_doublcmplx not &
        &firstprivatized"
        IF (scalar_double_complex /= (5e+9, 5e+9)) PRINT*, "The value&
        & of scalar_double_complex changed and it should not have"
#endif

        !$omp target defaultmap(tofrom: scalar)
          scalar_char = 'b'
#ifdef DOUBLE_COMPLEX
          scalar_double_complex = (10e+9, 10e+9)
#endif
        !$omp end target 

        IF (scalar_char /= 'b') PRINT*, "The value of scalar_char &
        &did not changed and it should have"
#ifdef DOUBLE_COMPLEX
        IF (scalar_double_complex /= (5e+9, 5e+9)) PRINT*, "The value &
        & of scalar_double_complex did not changed and it should have"
#endif

      END PROGRAM
