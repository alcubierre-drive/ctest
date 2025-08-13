#define CT_SUITE_NAME newsuite
#include <ctest.h>

CT_TEST( nyos ) {
    CT_CHECK( 1 );
    /* CT_CHECK( 0 ); */
}
CT_TEST( nnos ) {
    CT_CHECK( 1 );
    CT_CHECK( 1 == 0 );
}
CT_TEST( ndos ) {
}

CT_TEST_HIDDEN( surprise ) {
    CT_CHECK( 1 );
    /* CT_CHECK( 0 ); */
}
