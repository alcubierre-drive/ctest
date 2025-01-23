#include "ctest.h"

INI_SUITE( TESTSUITE, yos ) {
    ASSERT( 1 );
    /* ASSERT( 0 ); */
}
NEXT_CASE( TESTSUITE, nos, yos ) {
    ASSERT( 1 );
    /* ASSERT( 0 ); */
}
NEXT_CASE( TESTSUITE, dos, nos ) {
    char* ptr = NULL;
    char** xptr = &ptr;
    printf( "%s\n", *xptr );
}
END_SUITE( TESTSUITE, dos )

