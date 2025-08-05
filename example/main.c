#define CT_MAIN
#include <ctest.h>

// this file include an argument parser, which soon will be part of ctest
int main(int argc, char** argv) {
    return ct_run(argv+1);
}
