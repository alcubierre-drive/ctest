# ctest
### Simple testing framework for C

To use ``ctest.h``, just include it in your test sources, e.g.,

```
#define CT_SUITE_NAME "my_suite"
#include <ctest.h>
CT_TEST( first_test ) {
    CT_CHECK( 1 ); // success
    CT_CHECK( 0 ); // failure
}
```

This defines the test function ``first_test`` with two assertions, within the
"suite" ``my_suite``. Each test source file becomes its own suite. In one of the
test files, or in a separate file, you have to define ``CT_MAIN``, such that
global variables are defined. You can then run all suites as follows

```
#define CT_MAIN
#include <ctest.h>

int main( int argc, char** argv ) {
    (void) argc;
    char** filters = argv+1;
    return ct_run(filters);
}
```

The filters are used to select either suites or specific test cases within a
suite. If ``*filters == NULL``, no filters are applied and all tests are run.

In default mode, ctest runs all suites in a separate process (via ``fork()``),
such that crashes can be recorded and contained in the output. This behavior can
be changed by defining ``CT_NO_FORK``.

See example [in the repo](example/)
