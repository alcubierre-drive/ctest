#include "../ctest.h"

INI_SUITE( TESTSUITE, yos ) {
    CTEST_CHK( 1 );
    /* CTEST_CHK( 0 ); */
}
NEXT_CASE( TESTSUITE, nos, yos ) {
    CTEST_CHK( 1 );
    /* CTEST_CHK( 0 ); */
}
NEXT_CASE( TESTSUITE, dos, nos ) {
    char* ptr = NULL;
    char** xptr = &ptr;
    printf( "%s\n", *xptr );
}
END_SUITE( TESTSUITE, dos )

