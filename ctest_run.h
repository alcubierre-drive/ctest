#pragma once

/** AUTOGENERATED BY 'ctest_headers.sh' **
 *
 * HEADER FILE THAT IMPLEMENTS THE TESTING FRAMEWORK GIVEN TEST SUITES IN THE
 * DIRECTORY 'example/test/'. THE MAIN TESTING FUNCTION IS DECLARED AS
 *
 * ctest_run( int* pargc, char*** pargv );
 *
 * NOTE THAT YOU CAN CHOOSE TO NOT ALLOW FORKING BY DEFINING 'CTEST_NO_FORK'
 * BEFORE INCLUDING THIS HEADER FILE ('ctest_run.h').
 */

#include <ctest.h>

testsuite_t register_newsuite( void );
testsuite_t register_TESTSUITE( void );

static int ctest_run( int argc, char** argv ) {
    testsuite_t suites[2+1] = {0};
    unsigned nsuites = 0;
    suites[nsuites++] = register_newsuite();
    suites[nsuites++] = register_TESTSUITE();

    char** filters = argv;
    filters[argc] = NULL;
    filters++; // skip argv[0]
    return ctest_run_suites( suites, nsuites, filters );
}
