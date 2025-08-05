#define CT_SUITE_NAME "TESTSUITE"
#include <ctest.h>

CT_TEST( yos ) {
    CT_CHECK( 1 );
    /* CT_CHECK( 0 ); */
}
CT_TEST( nos ) {
    CT_CHECK( 1 );
    /* CT_CHECK( 0 ); */
}
CT_TEST( dos ) {
    char* ptr = NULL;
    char** xptr = &ptr;
    printf( "%s\n", *xptr );
}

