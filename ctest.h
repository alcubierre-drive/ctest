#pragma once

#include <stdio.h>
#include <string.h>

typedef struct {
    const char* name;
    void (*run)( unsigned* nassert, unsigned* npassed, unsigned* ncases,
            unsigned* ncases_run, char** filters );
} testsuite_t;

static const char ctest_color_red[] = "\x1b[31;1m",
                  ctest_color_green[] = "\x1b[32;1m",
                  ctest_color_yellow[] = "\x1b[33;1m",
                  ctest_color_reset[] = "\033[0m";

#ifndef CTEST_PRINTF
#define CTEST_PRINTF printf
#endif

#define GENERATE_SUITE_TYPES(SUITE) \
typedef struct SUITE##_testfunc_t SUITE##_testfunc_t; \
 \
struct SUITE##_testfunc_t { \
    SUITE##_testfunc_t* prev; \
    const char* name; \
    void (*fun)( void ); \
};

#define NEXT_CASE( SUITE, NAME, PREV ) \
    static void SUITE##_##NAME( void ); \
    static SUITE##_testfunc_t SUITE##_testfunc_##NAME = { &SUITE##_testfunc_##PREV, #NAME, &SUITE##_##NAME }; \
    static void SUITE##_##NAME( void )

#define INI_SUITE( SUITE, NAME ) \
    GENERATE_SUITE_TYPES(SUITE) \
    static unsigned __nproblems = 0; \
    static unsigned __nasserts = 0; \
    static SUITE##_testfunc_t SUITE##_testfunc___endtests = { NULL, "__endtests", NULL }; \
    NEXT_CASE( SUITE, NAME, __endtests )

#define END_SUITE( SUITE, PREV ) \
    NEXT_CASE( SUITE, __starttests, PREV ) { \
        CTEST_PRINTF( "%s=== ctest suite '" #SUITE "' ===%s\n", \
                ctest_color_yellow, ctest_color_reset ); \
    } \
    void SUITE( unsigned* nassert, unsigned* npassed, unsigned* ncases, \
            unsigned* ncases_run, char** filters ) { \
        SUITE##_testfunc_t* tf = &SUITE##_testfunc___starttests; \
        unsigned nc = 0, nc_run = 0; \
        for (; tf->prev != NULL; tf=tf->prev) { \
            if (strcmp(tf->name, "__starttests")) { \
                int matches_case_filters = 0; \
                int matches_suite_filters = 0; \
                int nfilters = 0; \
                char** f = filters; \
                while (*f) { \
                    nfilters++; \
                    if (!strcmp(tf->name, *f)) matches_case_filters++; \
                    if (!strcmp(#SUITE, *f)) matches_suite_filters++; \
                    f++; \
                } \
                if (matches_suite_filters || matches_case_filters || nfilters==0) { \
                    CTEST_PRINTF( "... run test '%s'\n", tf->name ); \
                    nc_run++; \
                    tf->fun(); \
                } \
                nc++; \
            } else { \
                tf->fun(); \
            } \
        } \
        *nassert = __nasserts; \
        *npassed = __nasserts-__nproblems; \
        *ncases = nc; \
        *ncases_run = nc_run; \
    } \
    testsuite_t register_##SUITE( void ) { \
        return (testsuite_t){ .name = #SUITE, .run = &SUITE }; \
    }

#define ASSERT( expr ) { \
    __nasserts++; \
    if (!(expr)) { \
        __nproblems++; \
        CTEST_PRINTF( "%s!!! FAILED in '%s' (%s:%i): " #expr "%s\n", ctest_color_red, \
                __func__, __FILE__, __LINE__, ctest_color_reset ); \
    } \
}


