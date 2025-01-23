#include "ctest.h"

INI_SUITE( newsuite, yos ) {
    ASSERT( 1 );
    /* ASSERT( 0 ); */
}
NEXT_CASE( newsuite, nos, yos ) {
    ASSERT( 1 );
    ASSERT( 1 == 0 );
}
NEXT_CASE( newsuite, dos, nos ) {
}
END_SUITE( newsuite, dos )

