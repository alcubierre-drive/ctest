#include "../ctest.h"

INI_SUITE( newsuite, yos ) {
    CTEST_CHK( 1 );
    /* CTEST_CHK( 0 ); */
}
NEXT_CASE( newsuite, nos, yos ) {
    CTEST_CHK( 1 );
    CTEST_CHK( 1 == 0 );
}
NEXT_CASE( newsuite, dos, nos ) {
}
END_SUITE( newsuite, dos )

