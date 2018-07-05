      PROGRAM default_map_rep
        USE iso_fortran_env
        implicit none
        INTEGER:: placeholder
        COMPLEX(kind = 16) :: scalar_double_complex
        LOGICAL :: firstprivateCheck

        scalar_double_complex = (1e+10, 1e+10)
        
        !$omp target map(from: firstprivateCheck)
          ! Checking the 'firstprivate behavior'
          placeholder = placeholder
#ifdef ACTIVATE_BUG
          firstprivateCheck = scalar_double_complex == (1e+10, 1e+10)
          scalar_double_complex = (5e+9, 5e+9)
#endif
        !$omp end target 

        IF (.not. firstprivateCheck) PRINT*, "scalar_doublcmplx not &
        &firstprivatized"
        IF (scalar_double_complex /= (5e+9, 5e+9)) PRINT*, "The value&
        & of scalar_double_complex changed and it should not have"

        !$omp target defaultmap(tofrom: scalar)
          placeholder = placeholder
#ifdef ACTIVATE_BUG
          scalar_double_complex = (10e+9, 10e+9)
#endif
        !$omp end target 

        IF (scalar_double_complex /= (5e+9, 5e+9)) PRINT*, "The value &
        & of scalar_double_complex did not changed and it should have"

      END PROGRAM
