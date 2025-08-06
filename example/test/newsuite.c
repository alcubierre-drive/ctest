#define CT_SUITE_NAME newsuite
#include <ctest.h>

CT_TEST( yos ) {
    CT_CHECK( 1 );
    /* CT_CHECK( 0 ); */
}
CT_TEST( nos ) {
    CT_CHECK( 1 );
    CT_CHECK( 1 == 0 );
}
CT_TEST( dos ) {
}

